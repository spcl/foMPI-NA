/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *
 *  (C) 2012 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fompi.h"
#include <mpi.h>
#include "mpitest.h"

#define ITER 100

const int verbose = 0;

int main(int argc, char **argv) {
    int       i, rank, nproc;
    int       errors = 0, all_errors = 0;
    int       val = 0, one = 1;
    MPI_Aint *val_ptrs;
    foMPI_Win   dyn_win;

    foMPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    val_ptrs = malloc(nproc * sizeof(MPI_Aint));
    MPI_Get_address(&val, &val_ptrs[rank]);

    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, val_ptrs, 1, MPI_AINT,
                  MPI_COMM_WORLD);

    foMPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &dyn_win);
    foMPI_Win_attach(dyn_win, &val, sizeof(int));

    for (i = 0; i < ITER; i++) {
            foMPI_Win_fence(MPI_MODE_NOPRECEDE, dyn_win);
            foMPI_Accumulate(&one, 1, MPI_INT, i%nproc, val_ptrs[i%nproc], 1, MPI_INT, foMPI_SUM, dyn_win);
            foMPI_Win_fence(MPI_MODE_NOSUCCEED, dyn_win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /* Read and verify my data */
    if ( val != ITER ) {
        errors++;
        printf("%d -- Got %d, expected %d\n", rank, val, ITER);
    }

    foMPI_Win_detach(dyn_win, &one);
    foMPI_Win_free(&dyn_win);

    MPI_Reduce(&errors, &all_errors, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0 && all_errors == 0)
        printf(" No Errors\n");

    free(val_ptrs);
    foMPI_Finalize();

    return 0;
}
