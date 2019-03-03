/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
/* based on the pack.c test in the mpich suite.
 */

#include "mpi.h"
#include "mpitypes.h"
#include "mpitypes-config.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

static int verbose = 0;

#define BUF_SIZE 16384

int main(int argc, char *argv[]);
int parse_args(int argc, char **argv);

int main(int argc, char *argv[])
{
    int errs = 0;
    char buffer[BUF_SIZE];
    int n, pos;
    double a,b;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    parse_args(argc, argv);

    pos	= 0;
    n	= 10;
    a	= 1.1;
    b	= 2.2;

    MPIT_Type_memcpy(&n, 1, MPI_INT, buffer, MPIT_MEMCPY_FROM_USERBUF, 0, sizeof(int));

    if ( *(int *) buffer != n) {
    	errs++;
	if (verbose) fprintf(stderr, "Wrong value for n in packbuf.\n");
    }

    n = 0;

    MPIT_Type_memcpy(&n, 1, MPI_INT, buffer, MPIT_MEMCPY_TO_USERBUF, 0, sizeof(int));

    if ( n != 10) {
    	errs++;
	if (verbose) fprintf(stderr, "Wrong value for n in variable.\n");
    }


    MPIT_Type_memcpy(&a, 1, MPI_DOUBLE, buffer, MPIT_MEMCPY_FROM_USERBUF, 0, sizeof(double));

    if ( *(double *) buffer != a) {
    	errs++;
	if (verbose) fprintf(stderr, "Wrong value for a in packbuf.\n");
    }

    a = 0.0;

    MPIT_Type_memcpy(&a, 1, MPI_DOUBLE, buffer, MPIT_MEMCPY_TO_USERBUF, 0, sizeof(double));

    if ( a != 1.1) {
    	errs++;
	if (verbose) fprintf(stderr, "Wrong value for a in variable.\n");
    }

    MPIT_Type_memcpy(&b, 1, MPI_DOUBLE, buffer, MPIT_MEMCPY_FROM_USERBUF, 0, sizeof(double));

    if ( *(double *) buffer != b) {
    	errs++;
	if (verbose) fprintf(stderr, "Wrong value for b in packbuf.\n");
    }

    b = 0.0;

    MPIT_Type_memcpy(&b, 1, MPI_DOUBLE, buffer, MPIT_MEMCPY_TO_USERBUF, 0, sizeof(double));

    if ( b != 2.2) {
    	errs++;
	if (verbose) fprintf(stderr, "Wrong value for b in variable.\n");
    }

    /* print message and exit */
    if (errs) {
	fprintf(stderr, "Found %d errors\n", errs);
    }

    MPI_Finalize();
    return errs ? 1 : 0;
}

int parse_args(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-v") == 0)
	verbose = 1;
    return 0;
}
