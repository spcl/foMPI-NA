#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_put_fence( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int commsize;
  int rank;
  int status;
  int i;
  MPI_Aint start;

  int flag, create_flavor;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );
  
  for( i=0 ; i<3*DIM ; i++) {
    base[i] = commsize+i;
  }

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    MPI_Get_address( &base[DIM], &start);
    MPI_Sendrecv_replace( &start, 1, MPI_AINT, commsize - rank - 1, 0, commsize - rank - 1, 0, comm, MPI_STATUS_IGNORE );
  } else {
    start = DIM;
  }


  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Put( &base[0], DIM, MPI_INT, commsize - rank - 1, start, DIM, MPI_INT, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_fence( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  for( i=0 ; i<DIM ; i++) {
    if(base[i] != commsize+i) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize+i);
      exit(EXIT_FAILURE);
    }
  }
  for( i=0 ; i<DIM ; i++) {
    if(base[DIM+i] != commsize+i) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, DIM+i, base[DIM+i], commsize+i);
      exit(EXIT_FAILURE);
    }
  }
  for( i=2*DIM ; i<3*DIM ; i++) {
    if(base[i] != commsize+i) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], commsize+i);
      exit(EXIT_FAILURE);
    }
  }
  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
