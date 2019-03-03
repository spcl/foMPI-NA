/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
/* Based on code from Jeff Parker at IBM. */

#include <stdio.h>

#include "mpi.h"
#include "mpitypes.h"
#include "mpitypes-config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

static int verbose = 0;

/* helper functions */
static int parse_args(int argc, char **argv);

int main(int argc, char *argv[])
{
    MPI_Datatype mystruct, vecs[3];
    MPI_Aint stride = 5, displs[3];
    int i=0, blockcount[3];
    int err, errs=0;

    MPI_Init(&argc, &argv);
    parse_args(argc, argv);

    for(i = 0; i < 3; i++)
    {
        MPI_Type_hvector(i, 1, stride, MPI_INT, &vecs[i]);
        MPI_Type_commit(&vecs[i]);
        blockcount[i]=1;
    }
    displs[0]=0; displs[1]=-100; displs[2]=-200; /* irrelevant */

    MPI_Type_struct(3, blockcount, displs, vecs, &mystruct);
    MPI_Type_commit(&mystruct);
    err = MPIT_Type_init(mystruct);
    if (err != MPI_SUCCESS) {
        errs++;
	if (verbose) fprintf(stderr, "MPIT_Type_init returned error code %d\n",
			     err);
        return errs;
    }

    MPI_Type_free(&mystruct);
    for(i = 0; i < 3; i++)
    {
        MPI_Type_free(&vecs[i]);
    }

    /* this time with the first argument always 0 */
    for(i = 0; i < 3; i++)
    {
        MPI_Type_hvector(0, 1, stride, MPI_INT, &vecs[i]);
        MPI_Type_commit(&vecs[i]);
        blockcount[i]=1;
    }
    displs[0]=0; displs[1]=-100; displs[2]=-200; /* irrelevant */

    MPI_Type_struct(3, blockcount, displs, vecs, &mystruct);
    MPI_Type_commit(&mystruct);
    err = MPIT_Type_init(mystruct);
    if (err != MPI_SUCCESS) {
        errs++;
	if (verbose) fprintf(stderr, "MPIT_Type_init returned error code %d\n",
			     err);
        return errs;
    }

    MPI_Type_free(&mystruct);
    for(i = 0; i < 3; i++)
    {
        MPI_Type_free(&vecs[i]);
    }

    /* print message and exit */
    if (errs) {
	fprintf(stderr, "Found %d errors\n", errs);
    }

    MPI_Finalize();
    return errs ? 1 : 0;
}

static int parse_args(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-v") == 0)
	verbose = 1;
    return 0;
}
