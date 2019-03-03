#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_cswap_special_lock( int64_t* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int64_t* cswap_result;
  int64_t compare;
  int64_t cswap_buf;

  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  base[0] = rank % 2;
  compare = 0;
  cswap_buf = 1;

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    MPI_Get_address( &base[0], &start);
    if (rank == 0) {
      base_adr = malloc( commsize * sizeof(MPI_Aint) );
    }
    MPI_Gather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, 0, comm );
  } else {
    start = 0;
  }

  MPI_Barrier( comm ); /* guarantees that the initialization is finished */

  if (rank == 0) {
    cswap_result = malloc( commsize * sizeof(int64_t) );
    
    for( i = 0 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_SHARED, i, 0, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

    for( i = 0 ; i<commsize ; i++ ) {
      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
        status = foMPI_Compare_and_swap( &cswap_buf, &compare, &cswap_result[i], MPI_INT64_T, i, base_adr[i], win );
      } else {
        status = foMPI_Compare_and_swap( &cswap_buf, &compare, &cswap_result[i], MPI_INT64_T, i, start, win );
      }
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
  
    for( i = 0 ; i<commsize ; i++ ) {
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }

    for( i = 0 ; i<commsize ; i++ ) {
      if ( cswap_result[i] != (i % 2) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: MPI_Compare_swap error - result %li doesn't match %i\n", rank, __FILE__, __LINE__, cswap_result[i], (i % 2) );
        exit(EXIT_FAILURE);
      }
    }

    free( cswap_result );
    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      free( base_adr );
    }
  }
 
  MPI_Barrier( comm ); /* wait for the end of the accesses */

  if ( base[0] != 1 ) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %li instead of %i.\n", rank, __FILE__, __LINE__, 0, base[0], 1 );
    exit(EXIT_FAILURE);
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
