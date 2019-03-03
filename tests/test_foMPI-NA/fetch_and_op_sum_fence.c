#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_fetch_and_op_sum_fence( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int* result = NULL; /* to prevent compiler warnings */
  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  base[0] = rank;

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0 ) {
      base_adr = malloc( commsize * sizeof(MPI_Aint) );
    }
    MPI_Get_address( &base[0], &start);
    MPI_Gather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, 0, comm );
  } else {
    start = 0;
  }

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if (rank == 0) {
    base[2] = 1;

    result = malloc( commsize * sizeof(int) );

    for( i = 0 ; i<commsize ; i++ ) {
      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
        status = foMPI_Fetch_and_op( &base[2], &result[i], MPI_INT, i, base_adr[i], foMPI_SUM, win ); 
      } else {
        status = foMPI_Fetch_and_op( &base[2], &result[i], MPI_INT, i, start, foMPI_SUM, win );
      }
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      free( base_adr );
    }
  }
 
  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  if (rank == 0 ) {
    for( i = 0 ; i<commsize ; i++ ) {
      if ( result[i] != i ) {
        fprintf(stderr, "Error on rank %i in %s line %i: MPI_Fetch_an_op error - result %i doesn't match %i\n", rank, __FILE__, __LINE__, result[i], i );
        exit(EXIT_FAILURE);
      }
    }
    free( result );
  }

  if ( base[0] != (rank+1) ) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 0, base[0], rank+1 );
    exit(EXIT_FAILURE);
  }

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
