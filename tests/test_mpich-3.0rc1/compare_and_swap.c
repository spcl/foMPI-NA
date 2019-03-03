/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *
 *  (C) 2012 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "mpitest.h"
#include "fompi.h"

static const int SQ_LIMIT = 10;
static       int SQ_COUNT = 0;

#define SQUELCH(X)                      \
  do {                                  \
    if (SQ_COUNT < SQ_LIMIT) {          \
      SQ_COUNT++;                       \
      X                                 \
    }                                   \
  } while (0)

#define ITER 100

const int verbose = 0;

int main(int argc, char **argv) {
    int       i, rank, nproc;
    int       errors = 0, all_errors = 0;
    int      *val_ptr;
    foMPI_Win   win;

    foMPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    val_ptr = malloc(sizeof(int));

    *val_ptr = 0;

    foMPI_Win_create(val_ptr, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    /* Test self communication */

    for (i = 0; i < ITER; i++) {
        int next = i + 1, result = -1;
        foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, rank, 0, win);
        foMPI_Compare_and_swap(&next, &i, &result, MPI_INT, rank, 0, win);
        foMPI_Win_unlock(rank, win);
        if (result != i) {
            SQUELCH( printf("%d->%d -- Error: next=%d compare=%d result=%d val=%d\n", rank,
                           rank, next, i, result, *val_ptr); );
            errors++;
        }
    }

    foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, rank, 0, win);
    *val_ptr = 0;
    foMPI_Win_unlock(rank, win);

    MPI_Barrier(MPI_COMM_WORLD);

    /* Test neighbor communication */

    for (i = 0; i < ITER; i++) {
        int next = i + 1, result = -1;
        foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, (rank+1)%nproc, 0, win);
        foMPI_Compare_and_swap(&next, &i, &result, MPI_INT, (rank+1)%nproc, 0, win);
        foMPI_Win_unlock((rank+1)%nproc, win);
        if (result != i) {
            SQUELCH( printf("%d->%d -- Error: next=%d compare=%d result=%d val=%d\n", rank,
                           (rank+1)%nproc, next, i, result, *val_ptr); );
            errors++;
        }
    }

    fflush(NULL);

    MPI_Barrier(MPI_COMM_WORLD);
    foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, rank, 0, win);
    *val_ptr = 0;
    foMPI_Win_unlock(rank, win);
    MPI_Barrier(MPI_COMM_WORLD);


    /* Test contention */

    if (rank != 0) {
        for (i = 0; i < ITER; i++) {
            int next = i + 1, result = -1;
            foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, 0, 0, win);
            foMPI_Compare_and_swap(&next, &i, &result, MPI_INT, 0, 0, win);
            foMPI_Win_unlock(0, win);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0 && nproc > 1) {
        if (*val_ptr != ITER) {
            SQUELCH( printf("%d - Error: expected=%d val=%d\n", rank, ITER, *val_ptr); );
            errors++;
        }
    }

    foMPI_Win_free(&win);

    MPI_Reduce(&errors, &all_errors, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0 && all_errors == 0)
        printf(" No Errors\n");

    free(val_ptr);
    foMPI_Finalize();

    return 0;
}
