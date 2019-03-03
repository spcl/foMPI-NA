#include <assert.h>
#include <unistd.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_lock_exclusive_concurrency( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  if ( (commsize < 2) && (rank == 0) ) {
    printf("This test needs at least 2 processes.\n");
    exit(EXIT_FAILURE);
  }

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 1) {
      MPI_Get_address( &base[0], &start );
    }
    MPI_Bcast( &start, 1, MPI_AINT, 1, comm );
  } else {
    start = 0;
  }

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  base[0] = 0;

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Barrier( comm );

  if ( rank == 0 ) {
    status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    int test;

    do {
      status = foMPI_Win_lock( foMPI_LOCK_SHARED, 1, 0, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

      status = foMPI_Get( &test, 1, MPI_INT, 1 /*target_rank */, start /* disp */, 1 /* target_count */, MPI_INT, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

      status = foMPI_Win_unlock( 1, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      //printf("%i: got %i\n", rank, test);
    } while ( test == 0 ); 

    status = foMPI_Win_unlock( rank, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  if ( rank == 1 ) {

    usleep(100);

    status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    base[0] = 1;

    status = foMPI_Win_unlock( rank, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  MPI_Barrier( comm ); /* wait for all accesses to finish */

  if (rank == 0) {
    printf("No Error\n");
  }
}
