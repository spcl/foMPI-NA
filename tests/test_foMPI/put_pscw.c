#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_put_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int ranges[1][3];
  MPI_Group group;
  MPI_Group newgroup;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if (rank == 1) {
      MPI_Get_address( &base[3*DIM-1], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 1, comm );
  } else {
    start = 3*DIM-1;
  }

  ranges[0][0] = (rank+1) % 2;
  ranges[0][1] = commsize-1;
  ranges[0][2] = 2;
  MPI_Comm_group(comm, &group);
  MPI_Group_range_incl(group, 1, ranges, &newgroup);
  MPI_Group_free( &group );

  if ( (rank%2) == 0 ) {
    status = foMPI_Win_start(newgroup, 0, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
    base[0] = 4*DIM;
    status = foMPI_Put( (void*) &base[0], 1, MPI_INT, 1, start, 1, MPI_INT, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    status = foMPI_Win_complete(win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  } else {
    status = foMPI_Win_post(newgroup, 0, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

    status = foMPI_Win_wait(win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  MPI_Group_free( &newgroup );

  if ( rank == 1 ) {
    if (base[3*DIM-1] != 4*DIM) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 3*DIM-1, base[3*DIM-1], 4*DIM); 
      exit(EXIT_FAILURE);
    }
  }

  MPI_Barrier( comm );

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
