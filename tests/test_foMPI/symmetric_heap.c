#include "fompi.h"
#include "test_mpi3rma.h"

int main( int argc, char *argv[] ) {

  foMPI_Win win;
  int status;

  int* base;

  status = foMPI_Init( &argc, &argv );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_allocate( 15*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &base, &win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Finalize();
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  return 0;
}
