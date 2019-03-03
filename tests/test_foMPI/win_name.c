/* This test is derived from the win_name test of the RMA tests of mpich-1.5b2. */

#include <stdio.h>
#include <string.h>

#include "fompi.h"
#include "test_mpi3rma.h"

static int cnt = 0;

void test_mpi3rma_win_name( foMPI_Win win ) {

  char name[MPI_MAX_OBJECT_NAME];
  char getname[MPI_MAX_OBJECT_NAME];
  int length;
  int status;

  snprintf(name, MPI_MAX_OBJECT_NAME, "win-%d", cnt++);

  status = foMPI_Win_set_name( win, &name[0] );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  status = foMPI_Win_get_name( win, &getname[0], &length );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if( strncmp(name, getname, MPI_MAX_OBJECT_NAME) == 0 ) {
    printf("No Error\n");
  } else {
    printf("Error: Got name %s instead of %s.\n", getname, name);
  }

  return;
}
