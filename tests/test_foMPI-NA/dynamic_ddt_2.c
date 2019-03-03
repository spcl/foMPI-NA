#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_2( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int local_DIM = 5;
  int* send_buffer;

  MPI_Datatype origin_dtype;
  MPI_Datatype target_dtype;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  send_buffer = malloc( 3 * local_DIM * sizeof(int) );

  for( i=0 ; i<3*local_DIM ; i++) {
    send_buffer[i] = 3*local_DIM*rank+i;
    base[i] = -1;
  }
  
  MPI_Type_vector( 2, 4, 5, MPI_INT, &origin_dtype );
  MPI_Type_commit( &origin_dtype );

  MPI_Type_vector( 4, 2, 4, MPI_INT, &target_dtype );
  MPI_Type_commit( &target_dtype );

  MPI_Get_address( &base[0], &start );
  MPI_Sendrecv_replace( &start, 1, MPI_AINT, commsize-rank-1, 0, commsize-rank-1, 0, comm, MPI_STATUS_IGNORE );

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Put( &send_buffer[0], 1, origin_dtype, commsize-rank-1, start, 1, target_dtype, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &origin_dtype );
  foMPI_Type_free( &target_dtype );

  for( k=0 ; k<2 ; k++ ) {
    for( i=0 ; i<2 ; i++) {
      if( base[k*4+i] != 3*local_DIM*(commsize-rank-1)+2*k+i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 4*k+i, base[4*k+i], 3*local_DIM*(commsize-rank-1)+2*k+i );
        exit(EXIT_FAILURE);
      }
    }
    for( i=0 ; i<2 ; i++) {
      if( base[8+k*4+i] != 3*local_DIM*(commsize-rank-1)+2*k+5+i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 8+4*k+i, base[8+4*k+i], 3*local_DIM*(commsize-rank-1)+2*k+5+i );
        exit(EXIT_FAILURE);
      }
    }
  }
  for( k=0 ; k<3 ; k++ ) {
    for( i=0 ; i<2 ; i++) {
      if( base[k*4+2+i] != -1 ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, k*4+2+i, base[2+k*4+i], -1 );
        exit(EXIT_FAILURE);
      }
    }
  }
  if(base[3*local_DIM-1] != -1) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 3*local_DIM-1, base[3*local_DIM-1], -1);
    exit(EXIT_FAILURE);
  }
  free( send_buffer );

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
