#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_ddt_3( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Datatype origin_dtype;
  MPI_Datatype target_dtype;
  MPI_Datatype result_dtype;
  MPI_Datatype temp_dtype;

  int* recv_buf;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  recv_buf = malloc( 12 * sizeof(int) );

  for( k=0 ; k<3 ; k++ ) {
    for( i=0 ; i<2 ; i++ ) {
      base[k*3+i] = 2*k+i;
    }
  }
  if (rank == 0) {
    for( k=0 ; k<2 ; k++ ) {
      for( i=0 ; i<3 ; i++ ) {
        base[8+4*k+i] = k*3+i;
      }
    }
  }
  for( i=0 ; i<12 ; i++ ) {
    recv_buf[i] = 0;
  }

  MPI_Type_vector( 3, 2, 3, MPI_INT, &origin_dtype );
  MPI_Type_commit( &origin_dtype );
  MPI_Type_vector( 6, 1, 2, MPI_INT, &result_dtype );
  MPI_Type_commit( &result_dtype );
  MPI_Type_vector( 1, 3, 4, MPI_INT, &temp_dtype );
  MPI_Type_create_resized( temp_dtype, 0, 4*sizeof(int), &target_dtype );
  MPI_Type_commit( &target_dtype );
  status = foMPI_Type_free( &temp_dtype );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0) {
      MPI_Get_address( &base[8], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm );
  } else {
    start = 8;
  }

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  status = foMPI_Get_accumulate( &base[0], 1, origin_dtype, &recv_buf[0], 1, result_dtype, 0, start, 2, target_dtype, foMPI_SUM, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
 
  status = foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &origin_dtype );
  foMPI_Type_free( &result_dtype );
  foMPI_Type_free( &target_dtype );

  if ( rank == 0 ) {
    for( k=0 ; k<2 ; k++ ) {
      for( i=0 ; i<3 ; i++ ) {
        if( base[8+k*4+i] != ((commsize+1)*(k*3+i)) ) {
          fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 8+k*4+i, base[8+k*4+i], (commsize+1)*(k*3+i) );
          exit(EXIT_FAILURE);
        }
      }
    }
  }

  if( recv_buf[0] != 0) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 0, recv_buf[0], 0 );
    exit(EXIT_FAILURE);
  }
  if( recv_buf[1] != 0) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 1, recv_buf[1], 0 );
    exit(EXIT_FAILURE);
  }
  for( i=1 ; i<6 ; i++ ) {
    if( (recv_buf[2*i] % i) != 0) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i (%i) is not a multiple of %i.\n", rank, __FILE__, __LINE__, 2*i, recv_buf[2*i], i );
      exit(EXIT_FAILURE);
    }
    if( recv_buf[2*i+1] != 0) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i (%i) is not %i.\n", rank, __FILE__, __LINE__, 2*i+1, recv_buf[2*i+1], 0 );
      exit(EXIT_FAILURE);
    }
  }

  free( recv_buf );

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
