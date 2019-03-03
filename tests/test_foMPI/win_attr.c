#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_win_attr( MPI_Comm comm ) {

  char create_flavor[20];
  foMPI_Win win;
  int j;
  int status;

  char space[6];
  char* ptr;

  void* return_ptr;
  MPI_Aint return_size;
  int return_constant;

  int flag;

  for ( j=0 ; j<3 ; j++ ) {
    switch (j) {
      case 0: status = foMPI_Win_create(&space[0], 6*sizeof(char), 3*sizeof(char), MPI_INFO_NULL, comm, &win);
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              snprintf(create_flavor, 20, "create");

              status = foMPI_Win_get_attr( win, foMPI_WIN_BASE, &return_ptr, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_ptr != &space[0] ) {
                fprintf(stderr, "Error: Base address retrieved by MPI_Win_get_attr is %p instead of %p for MPI_Win_%s.\n", return_ptr, &space[0], create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_SIZE, &return_size, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_size != 6*sizeof(char) ) {
                fprintf(stderr, "Error: Window size retrieved by MPI_Win_get_attr is %lu instead of %lu for MPI_Win_%s.\n", return_size, 6*sizeof(char), create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_DISP_UNIT, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != 3*sizeof(char) ) {
                fprintf(stderr, "Error: Window displacement unit retrieved by MPI_Win_get_attr is %i instead of %lu for MPI_Win_%s.\n", return_constant, 3*sizeof(char), create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != foMPI_WIN_FLAVOR_CREATE ) {
                fprintf(stderr, "Error: Window create flavor retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, foMPI_WIN_FLAVOR_CREATE, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_MODEL, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != foMPI_WIN_UNIFIED ) {
                fprintf(stderr, "Error: Window memory model retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, foMPI_WIN_UNIFIED, create_flavor);
                exit(EXIT_FAILURE);
              }

              break;
      case 1: status = foMPI_Win_allocate( 15, 5, MPI_INFO_NULL, comm, &ptr, &win);
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              snprintf(create_flavor, 20, "allocate");
 
              status = foMPI_Win_get_attr( win, foMPI_WIN_BASE, &return_ptr, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_ptr != ptr ) {
                fprintf(stderr, "Error: Base address retrieved by MPI_Win_get_attr is %p instead of %p for MPI_Win_%s.\n", return_ptr, ptr, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_SIZE, &return_size, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_size != 15 ) {
                fprintf(stderr, "Error: Window size retrieved by MPI_Win_get_attr is %lu instead of %i for MPI_Win_%s.\n", return_size, 15, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_DISP_UNIT, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != 5 ) {
                fprintf(stderr, "Error: Window displacement unit retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, 5, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != foMPI_WIN_FLAVOR_ALLOCATE ) {
                fprintf(stderr, "Error: Window create flavor retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, foMPI_WIN_FLAVOR_ALLOCATE, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_MODEL, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != foMPI_WIN_UNIFIED ) {
                fprintf(stderr, "Error: Window memory model retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, foMPI_WIN_UNIFIED, create_flavor);
                exit(EXIT_FAILURE);
              }

              break;
      case 2: status = foMPI_Win_create_dynamic( MPI_INFO_NULL, comm, &win);
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              snprintf(create_flavor, 20, "create_dynamic");

              status = foMPI_Win_get_attr( win, foMPI_WIN_BASE, &return_ptr, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_ptr != MPI_BOTTOM ) {
                fprintf(stderr, "Error: Base address retrieved by MPI_Win_get_attr is %p instead of %p for MPI_Win_%s.\n", return_ptr, MPI_BOTTOM, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_SIZE, &return_size, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_size != 0 ) {
                fprintf(stderr, "Error: Window size retrieved by MPI_Win_get_attr is %lu instead of %i for MPI_Win_%s.\n", return_size, 0, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_DISP_UNIT, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != 1 ) {
                fprintf(stderr, "Error: Window displacement unit retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, 1, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != foMPI_WIN_FLAVOR_DYNAMIC ) {
                fprintf(stderr, "Error: Window create flavor retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, foMPI_WIN_FLAVOR_DYNAMIC, create_flavor);
                exit(EXIT_FAILURE);
              }

              status = foMPI_Win_get_attr( win, foMPI_WIN_MODEL, &return_constant, &flag );
              _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
              assert( flag != 0 );

              if ( return_constant != foMPI_WIN_UNIFIED ) {
                fprintf(stderr, "Error: Window memory model retrieved by MPI_Win_get_attr is %i instead of %i for MPI_Win_%s.\n", return_constant, foMPI_WIN_UNIFIED, create_flavor);
                exit(EXIT_FAILURE);
              }

              break;
    }
    status= foMPI_Win_free( &win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  printf("No Error\n");

  return;
}
