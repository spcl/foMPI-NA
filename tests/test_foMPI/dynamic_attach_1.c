#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_attach_1( int DIM, foMPI_Win win, MPI_Comm comm ) {

  int rank;
  int status;

  int* base;
  int* base2;
  int* base3;

  MPI_Comm_rank( comm, &rank );
  
  base = malloc( DIM * sizeof(int) );
  base2 = malloc( DIM * sizeof(int) );
  base3 = malloc( DIM * sizeof(int) );

  status = foMPI_Win_attach( win, base, DIM*sizeof(int) );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_attach( win, base2, DIM*sizeof(int) );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_attach( win, base3, DIM*sizeof(int) );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_detach( win, base3 );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_detach( win, base );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_detach( win, base2 );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  free( base );
  free( base2 );
  free( base3 );

  MPI_Barrier( comm );

  if ( rank == 0 ) {
     printf("No Error\n");
  }

  return;
}
