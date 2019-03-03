/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* One-Sided MPI 2-D Strided Accumulate Test
 *
 * Author: James Dinan <dinan@mcs.anl.gov> 
 * Date  : December, 2010
 *
 * This code performs one-sided accumulate into a 2d patch of a shared array.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fompi.h"
#include <mpi.h>
#include "mpitest.h"

#define XDIM 1024 
#define YDIM 1024
#define ITERATIONS 10

static int verbose = 0;

int main(int argc, char **argv) {
    int i, j, rank, nranks, peer, bufsize, errors;
    double *buffer, *src_buf;
    foMPI_Win buf_win;

    foMPI_Init(&argc, &argv);
    MTest_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    bufsize = XDIM * YDIM * sizeof(double);
    MPI_Alloc_mem(bufsize, MPI_INFO_NULL, &buffer);
    MPI_Alloc_mem(bufsize, MPI_INFO_NULL, &src_buf);

    if (rank == 0)
        if (verbose) printf("MPI RMA Strided Accumulate Test:\n");

    for (i = 0; i < XDIM*YDIM; i++) {
        *(buffer  + i) = 1.0 + rank;
        *(src_buf + i) = 1.0 + rank;
    }

    foMPI_Win_create(buffer, bufsize, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &buf_win);

    peer = (rank+1) % nranks;

    for (i = 0; i < ITERATIONS; i++) {

      foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, peer, 0, buf_win);

      for (j = 0; j < YDIM; j++) {
        foMPI_Accumulate(src_buf + j*XDIM, XDIM, MPI_DOUBLE, peer,
                       j*XDIM*sizeof(double), XDIM, MPI_DOUBLE, foMPI_SUM, buf_win);
      }

      foMPI_Win_unlock(peer, buf_win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    foMPI_Win_lock(foMPI_LOCK_EXCLUSIVE, rank, 0, buf_win);
    for (i = errors = 0; i < XDIM; i++) {
      for (j = 0; j < YDIM; j++) {
        const double actual   = *(buffer + i + j*XDIM);
        const double expected = (1.0 + rank) + (1.0 + ((rank+nranks-1)%nranks)) * (ITERATIONS);
        if (fabs(actual - expected) > 1.0e-10) {
          printf("%d: Data validation failed at [%d, %d] expected=%f actual=%f\n",
              rank, j, i, expected, actual);
          errors++;
          fflush(stdout);
        }
      }
    }
    foMPI_Win_unlock(rank, buf_win);

    foMPI_Win_free(&buf_win);
    MPI_Free_mem(buffer);
    MPI_Free_mem(src_buf);

    MTest_Finalize( errors );
    foMPI_Finalize();
    return MTestReturnValue( errors );
}
