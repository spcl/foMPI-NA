#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_acc_replace_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  for( i=0 ; i<DIM ; i++ ) {
    base[i] = rank+i;
  }
  if (rank == 0) {
    for( i=0 ; i<DIM ; i++ ) {
      base[DIM+i] = commsize+i;
    }
  }

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );

  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    base_adr = malloc( commsize * sizeof(MPI_Aint) );
    MPI_Get_address( &base[0], &start );
    MPI_Allgather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, comm ); /* the init phase is finished */
  } else {
    start = 0;
    MPI_Barrier( comm ); /* the init phase is finished */
  }

  if (rank == 0) {
    for( i=0 ; i<commsize ; i++ ) {
      status = foMPI_Win_lock( foMPI_LOCK_SHARED, i, 0, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      
      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
        status = foMPI_Accumulate( &base[DIM], DIM, MPI_INT, i, base_adr[i], DIM, MPI_INT, foMPI_REPLACE, win );
      } else {
        status = foMPI_Accumulate( &base[DIM], DIM, MPI_INT, i, start, DIM, MPI_INT, foMPI_REPLACE, win );
      }
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      
      status = foMPI_Win_unlock( i, win );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
  }

  MPI_Barrier( comm ); /* wait for all accesses to finish */

  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    free( base_adr );
  }

  for( i=0 ; i<DIM ; i++ ) {
    if( base[i] != (commsize+i) ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize+i );
      exit(EXIT_FAILURE);
    }
  }

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
