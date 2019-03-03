#include "fompi.h"

void test_mpi3rma_null_pointer(MPI_Comm comm) {

  int res;
  int res_free;
  char create_flavor[20];
  foMPI_Win win;
  int j;

  for ( j=0 ; j<2 ; j++ ) {
    /* test if a NULL pointer as base is valid */
    switch (j) {
      case 0: res = foMPI_Win_create(NULL, 0, 4, MPI_INFO_NULL, comm, &win);
              snprintf(create_flavor, 20, "create");
              break;
      case 1: res = foMPI_Win_allocate( 0, 4, MPI_INFO_NULL, comm, NULL, &win);
              snprintf(create_flavor, 20, "allocate");
              break;
    }
    res_free = foMPI_Win_free( &win );

    if ( (res == MPI_SUCCESS) && (res_free == MPI_SUCCESS) ) {
      printf("%s - No Error\n", create_flavor);
    } else {
      printf("Error: Return code of MPI_Win_%s was %i and of MPI_Win_free was %i instead of %i\n", create_flavor, res, res_free, MPI_SUCCESS);
    }
  }

  return;
}
