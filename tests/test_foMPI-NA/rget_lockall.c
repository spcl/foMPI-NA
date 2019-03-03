#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_rget_lockall( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Status status_rma;
  foMPI_Request* request;
  int min;
  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */
  
  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  base[0] = rank;

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
    MPI_Gather( &start, 1, MPI_AINT, base_adr, 1, MPI_AINT, 0, comm );
  } else {
    start = 0;
  }
  MPI_Barrier( comm ); /* guarantees that the initialization is finished */

  if (rank == 0) {
    request = malloc( commsize * sizeof(foMPI_Request) );
 
    if (commsize > 3*DIM) {
      min = 3*DIM;
    } else {
      min = commsize;
    }
    
    status = foMPI_Win_lock_all(0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    for( i=0 ; i<min ; i++ ) {
      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
        status = foMPI_Rget( &base[i], 1, MPI_INT, i, base_adr[i], 1, MPI_INT, win, &request[i]);
      } else {
        status = foMPI_Rget( &base[i], 1, MPI_INT, i, start, 1, MPI_INT, win, &request[i]);
      }
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
    
    status = foMPI_Win_unlock_all( win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    for( i=0 ; i<min ; i++ ) {
      flag = 2;
      status = foMPI_Test( &request[i], &flag, &status_rma );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      assert( (flag == 0) || (flag == 1) );
      if (flag != 1) {
        printf("%i: request %i was not finished after unlock\n", rank, i);
        exit(EXIT_FAILURE);
      }
    }
  
    free( request );

    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      free ( base_adr );
    }

    for( i=0 ; i<min ; i++) {
      if( base[i] != i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], i);
        exit(EXIT_FAILURE);
      }
    }
  }

  MPI_Barrier( comm ); /* wait for the end of the accesses */

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
