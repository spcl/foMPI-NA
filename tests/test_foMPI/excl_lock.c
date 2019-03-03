#include "fompi.h"
#include "test_mpi3rma.h"

/* this isn't one of the general tests, instead it is used to test some implementation details (list of exclusive locks),
 * including some ugly access of the window struct by the test itself 
 * so if this test breaks, if you change the implementation of exclusive lock bookkeeping, no worries */

void test_mpi3rma_excl_lock( MPI_Comm comm ) {

  int rank, commsize;
  int test = 0;
  int put_data = 1;
  foMPI_Win win;
  int i;
  int status;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  /* the part of the foMPI_Win_free implementation we want to test is the same of all three window types */
  status = foMPI_Win_create( &test, sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if (rank == 0 ) {

    for( i=0 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

      status = foMPI_Put( &put_data, 1, MPI_INT, i, 0, 1, MPI_INT, win ); /* so that we actually do something, in case we implement a lazy approach later */
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

    /* first test: see if we can remove those exclusive locks from the local list without problems, if we remove them inorder */
    for( i=0 ; i<commsize ; i++ ) {
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

  }

  MPI_Barrier( comm );

  if( test != put_data ) {
    fprintf(stderr, "Error on rank %i in %s line %i: element is %i instead of %i.\n", rank, __FILE__, __LINE__, test, put_data );
    exit(EXIT_FAILURE);
  }
  test = 0;

  if ( win->excl_locks != NULL ) { /* not really a clean solution */
    fprintf(stderr, "Error on rank %i in %s line %i: list of exclusive locks isn't empty.\n", rank, __FILE__, __LINE__ );
    exit(EXIT_FAILURE);
  }

  MPI_Barrier( comm );

  if (rank == 0 ) {

    for( i=0 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

      status = foMPI_Put( &put_data, 1, MPI_INT, i, 0, 1, MPI_INT, win ); /* so that we actually do something, in case we implement a lazy approach later */
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

    /* second test: see if we can remove those exclusive locks from the local list without problems, if we remove them in reverse order */
    for( i=commsize-1 ; i>=0 ; i-- ) {
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

  }

  MPI_Barrier( comm );

  if( test != put_data ) {
    fprintf(stderr, "Error on rank %i in %s line %i: element is %i instead of %i.\n", rank, __FILE__, __LINE__, test, put_data );
    exit(EXIT_FAILURE);
  }
  test = 0;

  if ( win->excl_locks != NULL ) { /* not really a clean solution */
    fprintf(stderr, "Error on rank %i in %s line %i: list of exclusive locks isn't empty.\n", rank, __FILE__, __LINE__ );
    exit(EXIT_FAILURE);
  }

  MPI_Barrier( comm );

  if (rank == 0 ) {

    for( i=0 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

      status = foMPI_Put( &put_data, 1, MPI_INT, i, 0, 1, MPI_INT, win ); /* so that we actually do something, in case we implement a lazy approach later */
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

    /* third test: see if we can remove those exclusive locks from the local list without problems, if we first remove the even ranks and then the odd ones */
    for( i=0 ; i<commsize ; i+=2 ) {
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
    for( i=1 ; i<commsize ; i+=2 ) {
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
  }

  MPI_Barrier( comm );

  if( test != put_data ) {
    fprintf(stderr, "Error on rank %i in %s line %i: element is %i instead of %i.\n", rank, __FILE__, __LINE__, test, put_data );
    exit(EXIT_FAILURE);
  }
  test = 0;

  if ( win->excl_locks != NULL ) { /* not really a clean solution */
    fprintf(stderr, "Error on rank %i in %s line %i: list of exclusive locks isn't empty.\n", rank, __FILE__, __LINE__ );
    exit(EXIT_FAILURE);
  }

  MPI_Barrier( comm );

  if (rank == 0 ) {

    for( i=0 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

      status = foMPI_Put( &put_data, 1, MPI_INT, i, 0, 1, MPI_INT, win ); /* so that we actually do something, in case we implement a lazy approach later */
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

    /* fourth test: no unlock of the exclusive locks, so this has to be done by foMPI_Win_free (no actual consistency),
     * just make sure that the implementation doesn't break at this point (and hopefully doesn't leak memory) */
  }

  status = foMPI_Win_free( &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  /* fifth test: just one exclusive lock isn't unlocked */

  status = foMPI_Win_create( &test, sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, MPI_INFO_NULL, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Put( &put_data, 1, MPI_INT, rank, 0, 1, MPI_INT, win ); /* so that we actually do something, in case we implement a lazy approach later */
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_free( &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  /* sixth test: without a exclusive lock */

  status = foMPI_Win_create( &test, sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_free( &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Barrier( comm );

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
