/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *
 *  (C) 2003 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h"
#include <mpi.h>
#include <stdio.h>
#include "mpitest.h"

#define ELEM_SIZE 8

int main( int argc, char *argv[] )
{
    int     rank;
    int     errors = 0, all_errors = 0;
    int    *flavor, *model, flag;
    void   *buf;
    foMPI_Win window;

    foMPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /** Create using MPI_Win_create() **/

    if (rank > 0)
      MPI_Alloc_mem(rank*ELEM_SIZE, MPI_INFO_NULL, &buf);
    else
      buf = NULL;

    foMPI_Win_create(buf, rank*ELEM_SIZE, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &window);
    foMPI_Win_get_attr(window, foMPI_WIN_CREATE_FLAVOR, &flavor, &flag);

    if (!flag) {
      printf("%d: MPI_Win_create - Error, no flavor\n", rank);
      errors++;
    } else if (*flavor != foMPI_WIN_FLAVOR_CREATE) {
      printf("%d: MPI_Win_create - Error, bad flavor (%d)\n", rank, *flavor);
      errors++;
    }

    foMPI_Win_get_attr(window, MPI_WIN_MODEL, &model, &flag);

    if (!flag) {
      printf("%d: MPI_Win_create - Error, no model\n", rank);
      errors++;
    } else if ( ! (*model == foMPI_WIN_SEPARATE || *model == foMPI_WIN_UNIFIED) ) {
      printf("%d: MPI_Win_create - Error, bad model (%d)\n", rank, *model);
      errors++;
    }

    foMPI_Win_free(&window);

    if (buf)
      MPI_Free_mem(buf);

    /** Create using MPI_Win_allocate() **/

    foMPI_Win_allocate(rank*ELEM_SIZE, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &buf, &window);

    if (rank > 0 && buf == NULL) {
      printf("%d: MPI_Win_allocate - Error, bad base pointer\n", rank);
      errors++;
    }

    foMPI_Win_get_attr(window, foMPI_WIN_CREATE_FLAVOR, &flavor, &flag);

    if (!flag) {
      printf("%d: MPI_Win_allocate - Error, no flavor\n", rank);
      errors++;
    } else if (*flavor != foMPI_WIN_FLAVOR_ALLOCATE) {
      printf("%d: MPI_Win_allocate - Error, bad flavor (%d)\n", rank, *flavor);
      errors++;
    }

    foMPI_Win_get_attr(window, foMPI_WIN_MODEL, &model, &flag);

    if (!flag) {
      printf("%d: MPI_Win_allocate - Error, no model\n", rank);
      errors++;
    } else if (*model != foMPI_WIN_SEPARATE && *model != foMPI_WIN_UNIFIED) {
      printf("%d: MPI_Win_allocate - Error, bad model (%d)\n", rank, *model);
      errors++;
    }

    foMPI_Win_free(&window);

    /** Create using MPI_Win_create_dynamic() **/

    foMPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &window);

    foMPI_Win_get_attr(window, foMPI_WIN_CREATE_FLAVOR, &flavor, &flag);

    if (!flag) {
      printf("%d: MPI_Win_create_dynamic - Error, no flavor\n", rank);
      errors++;
    } else if (*flavor != foMPI_WIN_FLAVOR_DYNAMIC) {
      printf("%d: MPI_Win_create_dynamic - Error, bad flavor (%d)\n", rank, *flavor);
      errors++;
    }

    foMPI_Win_get_attr(window, foMPI_WIN_MODEL, &model, &flag);

    if (!flag) {
      printf("%d: MPI_Win_create_dynamic - Error, no model\n", rank);
      errors++;
    } else if (*model != foMPI_WIN_SEPARATE && *model != foMPI_WIN_UNIFIED) {
      printf("%d: MPI_Win_create_dynamic - Error, bad model (%d)\n", rank, *model);
      errors++;
    }

    foMPI_Win_free(&window);

    MPI_Reduce(&errors, &all_errors, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0 && all_errors == 0)
        printf(" No Errors\n");

    foMPI_Finalize();

    return 0;
}