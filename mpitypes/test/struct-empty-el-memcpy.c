/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
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

/* tests */
int single_struct_test(void);

/* helper functions */
int parse_args(int argc, char **argv);

struct test_struct_1 {
    int a,b,c,d;
};

int main(int argc, char *argv[])
{
    int err, errs = 0;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    parse_args(argc, argv);

    /* To improve reporting of problems about operations, we
       change the error handler to errors return */
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, MPI_ERRORS_RETURN );

    err = single_struct_test();
    if (verbose && err) fprintf(stderr, "error in single_struct_test\n");
    errs += err;

    /* print message and exit */
    if (errs) {
	fprintf(stderr, "Found %d errors\n", errs);
    }

    MPI_Finalize();
    return errs ? 1 : 0;
}

int single_struct_test(void)
{
    int err, errs = 0;
    int sendbuf[6] = { 1, 2, 3, 4, 5, 6 };
    struct test_struct_1 ts1[2];
    MPI_Datatype mystruct;

    /* note: first element of struct has zero blklen and should be dropped */
    MPI_Aint disps[3]     = { 2*sizeof(float), 0,       2*sizeof(int) };
    int blks[3]           = { 0,               1,       2 };
    MPI_Datatype types[3] = { MPI_FLOAT,       MPI_INT, MPI_INT };

    ts1[0].a = -1;
    ts1[0].b = -1;
    ts1[0].c = -1;
    ts1[0].d = -1;

    ts1[1].a = -1;
    ts1[1].b = -1;
    ts1[1].c = -1;
    ts1[1].d = -1;

    err = MPI_Type_struct(3, blks, disps, types, &mystruct);
    if (err != MPI_SUCCESS) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "MPI_Type_struct returned error\n");
	}
    }

    MPI_Type_commit(&mystruct);

    err = MPIT_Type_init(mystruct);
    if (err != MPI_SUCCESS) {
        errs++;
	if (verbose) fprintf(stderr, "MPIT_Type_init returned error code %d\n",
			     err);
        return errs;
    }

    err = MPIT_Type_memcpy(ts1, 2, mystruct, sendbuf, MPIT_MEMCPY_TO_USERBUF,
			   0, 6*sizeof(int));
    if (err != MPI_SUCCESS) {
        errs++;
	if (verbose) fprintf(stderr, "MPIT_Type_memcpy returned error code %d\n",
			     err);
        return errs;
    }

    /* verify data */
    if (ts1[0].a != 1) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[0].a = %d; should be %d\n", ts1[0].a, 1);
	}
    }
    if (ts1[0].b != -1) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[0].b = %d; should be %d\n", ts1[0].b, -1);
	}
    }
    if (ts1[0].c != 2) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[0].c = %d; should be %d\n", ts1[0].c, 2);
	}
    }
    if (ts1[0].d != 3) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[0].d = %d; should be %d\n", ts1[0].d, 3);
	}
    }
    if (ts1[1].a != 4) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[1].a = %d; should be %d\n", ts1[1].a, 4);
	}
    }
    if (ts1[1].b != -1) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[1].b = %d; should be %d\n", ts1[1].b, -1);
	}
    }
    if (ts1[1].c != 5) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[1].c = %d; should be %d\n", ts1[1].c, 5);
	}
    }
    if (ts1[1].d != 6) {
	errs++;
	if (verbose) {
	    fprintf(stderr, "ts1[1].d = %d; should be %d\n", ts1[1].d, 6);
	}
    }

    MPI_Type_free(&mystruct);

    return errs;
}

/******************************************************************/

int parse_args(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-v") == 0)
	verbose = 1;
    return 0;
}
