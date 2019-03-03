/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2006 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h"
#include <stdio.h>
#include <mpi.h>
#include "mpitest.h"

int main(int argc, char* argv[])
{
  foMPI_Win win;
  MPI_Group group;
  int errs = 0;

  foMPI_Init(&argc,&argv);
  MTest_Init(&argc,&argv); 

  foMPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  foMPI_Win_get_group(win, &group);
  
  foMPI_Win_post(group, 0, win);
  foMPI_Win_start(group, 0, win);
  
  foMPI_Win_complete(win);
  
  foMPI_Win_wait(win);

  MPI_Group_free( &group );
  foMPI_Win_free(&win); 

  MTest_Finalize(errs);
  foMPI_Finalize();
  return 0;
}
