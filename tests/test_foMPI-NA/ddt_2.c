#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_ddt_2( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Datatype origin_dtype;
  MPI_Datatype target_dtype;
  
  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  for( i=0 ; i<7 ; i++ ) {
    base[i] = rank+i;
  }
  if (rank == 0) { 
    for( i=0 ; i<8 ; i++ ) {
      base[7+i] = 0;
    }
  }

  MPI_Type_vector(2, 3, 4, MPI_INT, &origin_dtype);
  MPI_Type_commit(&origin_dtype);

  MPI_Type_vector(3, 2, 3, MPI_INT, &target_dtype);
  MPI_Type_commit(&target_dtype);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0) {
      MPI_Get_address( &base[7], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm );
  } else {
    start = 7;
  }

  status = foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Accumulate( &base[0], 1, origin_dtype, 0, start, 1, target_dtype, foMPI_SUM, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
 
  status = foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &origin_dtype );
  foMPI_Type_free( &target_dtype );

  if ( rank == 0 ) {
    for( k=0 ; k<2 ; k++ ) {
      if( base[7+k] != ((commsize-1)*commsize/2+k*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 7+k, base[7+k], (commsize-1)*(commsize+1)/2+k*commsize );
        exit(EXIT_FAILURE);
      }
    }
    if( base[10] != ((commsize-1)*commsize/2+2*commsize) ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 10, base[10], (commsize-1)*((commsize-1)+1)/2+2*commsize );
      exit(EXIT_FAILURE);
    }
    if( base[11] != ((commsize-1)*commsize/2+4*commsize) ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 11, base[11], (commsize-1)*((commsize-1)+1)/2+4*commsize );
      exit(EXIT_FAILURE);
    }
    for( k=0 ; k<2 ; k++ ) {
      if( base[13+k] != ((commsize-1)*commsize/2+(5+k)*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 13+k, base[7+k], (commsize-1)*((commsize-1)+1)/2+(5+k)*commsize );
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
