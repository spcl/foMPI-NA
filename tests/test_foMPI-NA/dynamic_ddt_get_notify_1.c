#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_get_notify_1( int* base, foMPI_Win win, MPI_Comm comm ) {

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
  int tag = 0;
  int flag = 0;
  int receive_from = commsize-rank-1;
  int send_to = commsize-rank-1 ;
  foMPI_Request notification_request;
  MPI_Status notification_status;

  status = foMPI_Notify_init(win, receive_from , tag, 1, &notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  send_buffer = malloc( 3 * local_DIM * sizeof(int) );

  /*try also without registering memory */
  status = foMPI_Win_attach( win, send_buffer,  3 * local_DIM * sizeof(int) );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  for( i=0 ; i<3*local_DIM ; i++) {
    send_buffer[i] = 3*local_DIM*rank+i;
    base[i] = -1;
  }
  

  MPI_Type_vector( 2, 2, 3, MPI_INT, &temp_dtype );
  MPI_Type_vector( 2, 1, 2, temp_dtype, &target_dtype );
  MPI_Type_commit( &target_dtype );
  foMPI_Type_free( &temp_dtype );

  MPI_Get_address( send_buffer, &start );
  MPI_Sendrecv_replace( &start, 1, MPI_AINT, send_to, 0, receive_from, 0, comm, MPI_STATUS_IGNORE );

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
/*
  status = foMPI_Put_notify( &send_buffer[0], 8, MPI_INT, send_to, start, 1, target_dtype, win , tag);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
*/
  status = foMPI_Get_notify( &base[0], 1 , target_dtype, send_to, start, 8, MPI_INT , win , tag);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Start(&notification_request);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  status = foMPI_Wait(&notification_request, &notification_status);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Start(&notification_request);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  status = foMPI_Test(&notification_request, &flag, &notification_status);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_fence(0, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if(flag){
      fprintf(stderr, "Error on rank %i in %s line %i: Multiple notification on single get.\n", rank, __FILE__, __LINE__);
      exit(EXIT_FAILURE);
  }

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
  status = foMPI_Request_free(&notification_request);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_detach( win, send_buffer );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  free( send_buffer );

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
