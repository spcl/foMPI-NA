#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_get_fence( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int status;
  int i;
  int rank;
  int commsize;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  for( i=0 ; i<DIM ; i++ ) {
    base[i] = commsize+i;
    base[DIM+i] = commsize+i;
    base[2*DIM+i] = commsize+2*DIM+i;
  }

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 0 ) {
      MPI_Get_address( &base[DIM], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm );
  } else {
    start = DIM;
  }

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Get( (void*) &base[2*DIM], DIM, MPI_INT, 0, start, DIM, MPI_INT, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  for( i=0 ; i<15 ; i++) {
    if( base[i] != commsize+(i%DIM)) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize+(i%DIM));
      exit(EXIT_FAILURE);
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }
  return;
}
