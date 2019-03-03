#include <assert.h>
#include "fompi.h"
#include "test_mpi3rma.h"

#include <unistd.h>

void test_mpi3rma_rput_rget_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Status status_rma;
  foMPI_Request* request;
  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if (rank == 0) {
    for( i=0 ; i<DIM ; i++) {
      base[i] = commsize;
    }
  } else {
    for( i=0 ; i<(3*DIM) ; i++) {
      base[i] = 0;
    }
  }

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0) {
      base_adr = malloc( commsize * sizeof(MPI_Aint) );
    }
    MPI_Get_address( &base[0], &start);
    MPI_Gather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, 0, comm );
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm ); /* also the signal that the initialization is finished */
  } else {
    start = 0;
    MPI_Barrier( comm ); /* guarantees that the initialization is finished */
  }

  if (rank == 0) {
    
    request = malloc( commsize * sizeof(foMPI_Request) );
   
    for( i=1 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, 0, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
   
    for( i=1 ; i<commsize ; i++ ) {
      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
        status = foMPI_Rput( &base[0], DIM, MPI_INT, i, base_adr[i], DIM, MPI_INT, win, &request[i] );
      } else {
        status = foMPI_Rput( &base[0], DIM, MPI_INT, i, start, DIM, MPI_INT, win, &request[i] );
      }
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
    
    for( i=commsize-1 ; i>0 ; i-- ) {
      flag = 2;
      status = foMPI_Test( &request[i], &flag, &status_rma );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      assert( (flag == 0) || (flag == 1) );
      //if (flag == 1) { // if something is not finished during the epoch
      //  printf("%i: after rput request %i finished\n", rank, i);
      //}
    }
    
    for( i=1 ; i<commsize ; i++ ) {
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
    
    for( i=1 ; i<commsize ; i++ ) {
      if ( request[i] != foMPI_REQUEST_NULL ) {
        flag = 2;
        status = foMPI_Test( &request[i], &flag, &status_rma );
        _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
        assert( (flag == 0) || (flag == 1) );
        if (flag != 1) {
          printf("%i: request %i was not finished after unlock\n", rank, i);
          exit(EXIT_FAILURE);
        }
      }
    }
    
    free( request );
    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      free( base_adr );
    }
  } else {
    request = malloc( 2*sizeof(foMPI_Request) );
    
    status = foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    status = foMPI_Rget( &base[DIM], 1, MPI_INT, 0, start, 1, MPI_INT, win, &request[0]);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    status = foMPI_Rget( &base[DIM+1], 1, MPI_INT, 0, start, 1, MPI_INT, win, &request[1]);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
    status = foMPI_Wait( &request[1], &status_rma );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
    status = foMPI_Win_unlock( 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
    status = foMPI_Wait( &request[0], MPI_STATUS_IGNORE );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
    free( request );
  }

  MPI_Barrier( comm ); /* wait for the end of the accesses */

  if ( rank != 0 ) {
    for( i=0 ; i<DIM+2 ; i++) {
      if( base[i] != commsize ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize);
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
