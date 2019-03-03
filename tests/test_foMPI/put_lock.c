#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_put_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int min;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  if (rank == 0 ) {
    status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, 0, 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    for( i=0 ; i<DIM ; i++) {
      base[i] = 2*commsize+i;
      base[DIM+i] = 2*commsize+i;
      base[2*DIM+i] = 2*commsize+i;
    }

    status = foMPI_Win_unlock( 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0 ) {
      MPI_Get_address( &base[0], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm ); /* also the signal that the initialization is finished */
    start += rank * sizeof(int);
  } else {
    start = rank;
    MPI_Barrier( comm ); /* guarantees that the initialization is finished */
  }

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, 0, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  if (rank < (3*DIM)) {
    status = foMPI_Put( &commsize, 1, MPI_INT, 0, start, 1, MPI_INT, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }
  
  status = foMPI_Win_unlock( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Barrier( comm ); /* wait for the end of the accesses */

  if (rank == 0) {
    if ( 3*DIM > commsize ) {
      min = commsize;
    } else {
      min = 3*DIM;
    }
    for( i=0 ; i<min ; i++) {
      if( base[i] != commsize ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize);
        exit(EXIT_FAILURE);
      }
    }
    for( i=min ; i<3*DIM-1 ; i++) {
      if( base[i] != 2*commsize+(i%DIM)) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize+(i%DIM));
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
