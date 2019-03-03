#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_pscw1( foMPI_Win win, MPI_Comm comm ) {

  int rank;
  int commsize;
  int status;

  int ranges[1][3];
  MPI_Group group;
  MPI_Group newgroup;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  ranges[0][0] = (rank+1) % 2;
  ranges[0][1] = commsize-1;
  ranges[0][2] = 2;
  MPI_Comm_group(comm, &group);
  MPI_Group_range_incl(group, 1, ranges, &newgroup);
  MPI_Group_free( &group );

  status = foMPI_Win_start(newgroup, 0, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  status = foMPI_Win_post(newgroup, 0, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_complete(win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_wait(win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Group_free( &newgroup );

  MPI_Barrier( comm );

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
