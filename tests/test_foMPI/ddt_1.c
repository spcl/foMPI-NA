#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

/* test needs 15 elements, so local_DIM should be at least 5 */

void test_mpi3rma_ddt_1( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int* send_buffer;
  int local_DIM = 5;

  MPI_Datatype temp_dtype;
  MPI_Datatype origin_dtype;
  MPI_Datatype target_dtype;
  int type_size;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  send_buffer = malloc( local_DIM * 3 * sizeof(int) );

  for( i=0 ; i<3*local_DIM ; i++) {
    send_buffer[i] = 3*local_DIM*rank+i;
    base[i] = -1;
  }
  
  MPI_Type_contiguous(3, MPI_INT, &temp_dtype);
  MPI_Type_size(MPI_INT, &type_size);
  MPI_Type_create_resized( temp_dtype, 0, 4*type_size, &origin_dtype );
  MPI_Type_commit( &origin_dtype );
  foMPI_Type_free( &temp_dtype );

  MPI_Type_vector( 3, 4, 5, MPI_INT, &target_dtype );
  MPI_Type_commit( &target_dtype );

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    MPI_Get_address( &base[0], &start);
    MPI_Sendrecv_replace( &start, 1, MPI_AINT, commsize - rank - 1, 0, commsize - rank - 1, 0, comm, MPI_STATUS_IGNORE );
  } else {
    start = 0;
  }

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Put( &send_buffer[0], 4, origin_dtype, commsize - rank - 1, start, 1, target_dtype, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &origin_dtype );
  foMPI_Type_free( &target_dtype );

  for( k=0 ; k<3 ; k++ ) {
    for( i=0 ; i<3-k ; i++) {
      if( base[local_DIM*k+i] != 3*local_DIM*(commsize-rank-1)+local_DIM*k+i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, local_DIM*k+i, base[local_DIM*k+i], 3*local_DIM*(commsize-rank-1)+local_DIM*k+i );
        exit(EXIT_FAILURE);
      }
    }
    for( i=0 ; i<(k+1) ; i++) {
      if( base[3+k*4+i] != 3*local_DIM*(commsize-rank-1)+(k+1)*4+i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 3+k*4+i, base[3+k*4+i], 3*local_DIM*(commsize-rank-1)+(k+1)*4+i );
        exit(EXIT_FAILURE);
      }
    }
    if(base[(k+1)*local_DIM-1] != -1) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, (k+1)*local_DIM-1, base[(k+1)*local_DIM-1], -1);
      exit(EXIT_FAILURE);
    }
  }

  free( send_buffer );

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
