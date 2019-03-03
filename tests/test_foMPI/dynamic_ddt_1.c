#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_1( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int local_DIM = 5;
  int* send_buffer;

  MPI_Datatype temp_dtype;
  MPI_Datatype target_dtype;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  send_buffer = malloc( 3 * local_DIM * sizeof(int) );

  for( i=0 ; i<3*local_DIM ; i++) {
    send_buffer[i] = 3*local_DIM*rank+i;
    base[i] = -1;
  }
  
  MPI_Type_vector( 2, 2, 3, MPI_INT, &temp_dtype );
  MPI_Type_vector( 2, 1, 2, temp_dtype, &target_dtype );
  MPI_Type_commit( &target_dtype );
  foMPI_Type_free( &temp_dtype );

  MPI_Get_address( &base[0], &start );
  MPI_Sendrecv_replace( &start, 1, MPI_AINT, commsize-rank-1, 0, commsize-rank-1, 0, comm, MPI_STATUS_IGNORE );

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Put( &send_buffer[0], 8, MPI_INT, commsize-rank-1, start, 1, target_dtype, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &target_dtype );

  for( k=0 ; k<2 ; k++ ) {
    for( i=0 ; i<2 ; i++) {
      if( base[2*local_DIM*k+i] != 3*local_DIM*(commsize-rank-1)+4*k+i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 2*local_DIM*k+i, base[2*local_DIM*k+i], 3*local_DIM*(commsize-rank-1)+4*k+i );
        exit(EXIT_FAILURE);
      }
    }
    for( i=0 ; i<2 ; i++) {
      if( base[2*local_DIM*k+i+3] != 3*local_DIM*(commsize-rank-1)+k*4+2+i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 2*local_DIM*k+i+3, base[2*local_DIM*k+i+3], 3*local_DIM*(commsize-rank-1)+k*4+2+i );
        exit(EXIT_FAILURE);
      }
    }
    if( base[k*2*local_DIM+2] != -1) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, k*2*local_DIM+2, base[k*2*local_DIM+2], -1);
      exit(EXIT_FAILURE);
    }
  }
  for( i=local_DIM ; i<(2*local_DIM) ; i++) {
    if(base[i] != -1) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], -1);
      exit(EXIT_FAILURE);
    }
  }

  free( send_buffer );

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
