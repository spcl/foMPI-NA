#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_get_acc_maxloc_lock( float_int_t* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  float_int_t result[DIM];

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if ( ((commsize/2) == rank) || ((commsize/2+1) == rank) ) {
    for( i=0 ; i<DIM ; i++ ) {
      base[i].value = commsize*2+i;
      base[i].index = rank;
    }
  } else {
    for( i=0 ; i<DIM ; i++ ) {
      base[i].value = commsize+rank+i;
      base[i].index = rank;
    }
  }
  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0 ) {
      MPI_Get_address( &base[0], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm ); /* also the signal that the initialization is finished */
  } else {
    start = 0;
    MPI_Barrier( comm ); /* guarantees that the initialization is finished */
  }

  MPI_Barrier( MPI_COMM_WORLD );

  if (rank != 0 ) {
    status = foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      
    status = foMPI_Get_accumulate( base, DIM, MPI_FLOAT_INT, &result, DIM, MPI_FLOAT_INT, 0, start, DIM, MPI_FLOAT_INT, foMPI_MAXLOC, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
      
    status = foMPI_Win_unlock( 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }
  
  MPI_Barrier( comm ); /* wait for the end of the accesses */

  if (rank == 0) {
    for( i=0 ; i<DIM ; i++ ) {
      if ( (base[i].value != (commsize*2+i)) || (base[i].index != (commsize/2)) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: max element is %f instead of %f or index is %i instead of %i.\n", rank, __FILE__, __LINE__, base[i].value, (float) commsize*2+i, base[i].index, commsize/2);
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
