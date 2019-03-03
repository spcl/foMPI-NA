#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_get_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  for( i=0 ; i<DIM ; i++) {
    base[i] = commsize+i;
    base[DIM+i] = commsize+i;
    base[2*DIM+i] = commsize+i;
  }

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0 ) {
      MPI_Get_address( &base[3*DIM-1], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm ); /* also the signal that the initialization is finished */
  } else {
    start = 3*DIM-1;
    MPI_Barrier( comm ); /* also the signal that the initialization is finished */
  }

  status = foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Get( &base[0], 1, MPI_INT, 0, start, 1, MPI_INT, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_unlock( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Barrier( comm ); /* finished all accesses to this epoch */

  for( i=1 ; i<3*DIM-1 ; i++) {
    if ( base[i] != commsize+(i%DIM)) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize+(i%DIM));
      exit(EXIT_FAILURE);
    }
  }
  if ( base[0] != commsize+(DIM-1) ) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 0, base[0], commsize+(DIM-1));
    exit(EXIT_FAILURE);
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }
 
  return;
}
