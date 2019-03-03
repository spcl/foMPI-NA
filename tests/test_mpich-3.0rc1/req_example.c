/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *
 *  (C) 2003 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h"
#include <mpi.h>
#include <stdio.h>
#include <assert.h>
#include "mpitest.h"

#define NSTEPS 100
#define N 1000
#define M 10

/* This is example 11.21 from the MPI 3.0 spec:
 *
 * The following example shows how request-based operations can be used to
 * overlap communication with computation. Each process fetches, processes,
 * and writes the result for NSTEPS chunks of data. Instead of a single
 * buffer, M local buffers are used to allow up to M communication operations
 * to overlap with computation.
 */

/* Use a global variable to inhibit compiler optimizations in the compute
 * function. */
double junk = 0.0;

void compute(int step, double *data) {
    int i;

    for (i = 0; i < N; i++)
        junk += data[i] * (double) step;
}

int main( int argc, char *argv[] )
{
    int i, rank, nproc;
    int errors = 0, all_errors = 0;
    foMPI_Win win;
    foMPI_Request put_req[M] = { MPI_REQUEST_NULL };
    foMPI_Request get_req;
    double *baseptr;
    double data[M][N]; /* M buffers of length N */

    foMPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    assert(M < NSTEPS);

    foMPI_Win_allocate(NSTEPS*N*sizeof(double), sizeof(double), MPI_INFO_NULL,
                     MPI_COMM_WORLD, &baseptr, &win);

    foMPI_Win_lock_all(0, win);

    for (i = 0; i < NSTEPS; i++) {
        int target = (rank+1) % nproc;
        int j;

        /* Find a free put request */
        if (i < M) {
            j = i;
        } else {
            foMPI_Waitany(M, put_req, &j, MPI_STATUS_IGNORE);
        }

        foMPI_Rget(data[j], N, MPI_DOUBLE, target, i*N, N, MPI_DOUBLE, win,
                 &get_req);
        foMPI_Wait(&get_req,foMPI_STATUS_IGNORE);

        compute(i, data[j]);
        foMPI_Rput(data[j], N, MPI_DOUBLE, target, i*N, N, MPI_DOUBLE, win,
                 &put_req[j]);

    }

    foMPI_Waitall(M, put_req, MPI_STATUSES_IGNORE);
    foMPI_Win_unlock_all(win);

    foMPI_Win_free(&win);

    MPI_Reduce(&errors, &all_errors, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0 && all_errors == 0)
        printf(" No Errors\n");

    foMPI_Finalize();

    return 0;
}
