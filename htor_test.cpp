#include <stdlib.h>

#include "fompi.h"
#include "mpi.h"

#include <unistd.h>

int main( int argc, char **argv ) {

  int rank, commsize;
  int* ptr;
  MPI_Comm comm=MPI_COMM_WORLD;
  foMPI_Win win;
  
  foMPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank );
  MPI_Comm_size(MPI_COMM_WORLD, &commsize );

  foMPI_Win_allocate(sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &ptr, &win);

  foMPI_Win_fence( 0, win );

  ptr[0] = rank+1;

  printf("[%i] %i\n", rank, ptr[0]);
  //if(rank==0) myMPI_Put(&ptr[0], 1, MPI_INT, 1, 0, 1, MPI_INT, win);
  if(rank==0) foMPI_Accumulate(&ptr[0], 1, MPI_INT, 1, 0, 1, MPI_INT, foMPI_SUM, win);

  foMPI_Win_fence( 0, win );

  printf("after [%i] %i\n", rank, ptr[0]);

  foMPI_Win_free(&win);

  foMPI_Finalize();

  return 0;
}
