/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * Note: This version of the test uses an approach of creating a flattened
 *       representation of the type and manually copying pieces inside
 *       pack_and_upack(), testing the blockct and flatten functionalities.
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "mpitypes.h"
#include "mpitypes-config.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif

int main(int argc, char *argv[]);

/* helper functions */
static int pack_and_unpack(char *typebuf,
			   int count,
			   MPI_Datatype datatype,
			   int typebufsz);
int parse_args(int argc, char **argv);

static int verbose = 0;

int main(int argc, char *argv[])
{
    /* Variable declarations */
    int a[100][100];
    int disp[100], block[100];
    MPI_Datatype ltype;
	
    int bufsize;
    int i, j, err, errs = 0;
	
    /* Initialize a to some known values. */
    for(i = 0; i < 100; i++) {
	for(j = 0; j < 100; j++) {
	    a[i][j] = 1000*i + j;
	}
    }
	
    /* Initialize MPI */
    MPI_Init(&argc, &argv);
  
    parse_args(argc, argv);

    for(i = 0; i < 100; i++) {
	/* Fortran version has disp(i) = 100*(i-1) + i and block(i) = 100-i. */
	/* This code here is wrong. It compacts everything together,
	 * which isn't what we want.
	 * What we want is to put the lower triangular values into b and leave
	 * the rest of it unchanged, right?
	 */
	block[i] = i+1;
	disp[i] = 100*i;
    }
	
    /* Create datatype for lower triangular part. */
    MPI_Type_indexed(100, block, disp, MPI_INT, &ltype);
    MPI_Type_commit(&ltype);
	
    /* Pack it. */
    MPI_Type_size(ltype, &bufsize);
    err = pack_and_unpack((char *) a, 1, ltype, 100*100*sizeof(int));
    errs += err;
 
    for(i = 0; i < 100; i++) {
	for(j = 0; j < 100; j++) {
	    if (j > i && a[i][j] != 0) {
		errs++;
		if (verbose) fprintf(stderr, "a[%d][%d] = %d; should be %d\n",
				     i, j, a[i][j], 0);
	    }
	    else if (j <= i && a[i][j] != 1000*i + j) {
		errs++;
		if (verbose) fprintf(stderr, "a[%d][%d] = %d; should be %d\n",
				     i, j, a[i][j], 1000*i + j);
	    }
	}
    }

    MPI_Type_free(&ltype);

    /* print message and exit */
    if (errs) {
	fprintf(stderr, "Found %d errors\n", errs);
    }

    MPI_Finalize();
    return errs ? 1 : 0;
}

/******************************************************************/

/* pack_and_unpack()
 *
 * Perform packing and unpacking of a buffer for the purposes of checking
 * to see if we are processing a type correctly.  Zeros the buffer between
 * these two operations, so the data described by the type should be in
 * place upon return but all other regions of the buffer should be zero.
 *
 * Parameters:
 * typebuf - pointer to buffer described by datatype and count that
 *           will be packed and then unpacked into
 * count, datatype - description of typebuf
 * typebufsz - size of typebuf; used specifically to zero the buffer
 *             between the pack and unpack steps
 *
 */
static int pack_and_unpack(char *typebuf,
			   int count,
			   MPI_Datatype datatype,
			   int typebufsz)
{
    char *packbuf;
    int err, errs = 0, type_size;
    int i, packoff;
    MPI_Aint *disps, blockct;
    int *blocks, intblockct;

    err = MPI_Type_size(datatype, &type_size);
    if (err != MPI_SUCCESS) {
	errs++;
	if (verbose) {
	    fprintf(stderr,
		    "error in MPI_Type_size call; aborting after %d errors\n",
		    errs);
	}
	return errs;
    }

    type_size *= count;

    packbuf = (char *) malloc(type_size);
    if (packbuf == NULL) {
	errs++;
	if (verbose) {
	    fprintf(stderr,
		    "error in malloc call; aborting after %d errors\n",
		    errs);
	}
	return errs;
    }

    err = MPIT_Type_init(datatype);
    if (err) {
        errs++;
	if (verbose) fprintf(stderr, "MPIT_Type_init returned error code %d\n",
			     err);
        return errs;
    }

    /* get count of blocks so we can allocate space for flattened
       representation.
     */
    err = MPIT_Type_blockct(1, datatype, 0, type_size, &blockct);
    if (err != MPI_SUCCESS) {
        errs++;
	if (verbose) fprintf(stderr,
			     "MPIT_Type_blockct returned error code %d\n",
			     err);
        return errs;
    }

    /* allocate space for flattened representation */
    disps = (MPI_Aint *) malloc(blockct * sizeof(MPI_Aint));
    if (disps == NULL) {
	errs++;
	if (verbose) {
	    fprintf(stderr,
		    "error in malloc call; aborting after %d errors\n",
		    errs);
	}
	return errs;
    }

    blocks = (int *) malloc(blockct * sizeof(int));
    if (blocks == NULL) {
	errs++;
	if (verbose) {
	    fprintf(stderr,
		    "error in malloc call; aborting after %d errors\n",
		    errs);
	}
	return errs;
    }

    /* flatten */
    intblockct = blockct;
    err = MPIT_Type_flatten(typebuf, 1, datatype, 0, type_size, disps,
			   blocks, &intblockct);
    if (err != MPI_SUCCESS) {
	errs++;
	if (verbose) fprintf(stderr,
			     "MPIT_Type_flatten returned error code %d\n",
			     err);
	return errs;
    }

    /* copy into packbuf */
    packoff = 0;
    for (i=0; i < intblockct; i++) {
	memcpy(&packbuf[packoff], (char *) disps[i], blocks[i]);
	packoff += blocks[i];
    }

    memset(typebuf, 0, typebufsz);

    /* use MPIT_Type_memcpy() to copy back into the original buffer */
    err = MPIT_Type_memcpy(typebuf, 1, datatype, packbuf,
			   MPIT_MEMCPY_TO_USERBUF,
			   0, type_size);
    if (err != MPI_SUCCESS) {
        errs++;
	if (verbose) fprintf(stderr,
			     "MPIT_Type_memcpy returned error code %d (unpack)\n",
			     err);
        return errs;
    }

    free(packbuf);
    free(disps);
    free(blocks);

    return errs;
}

int parse_args(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-v") == 0)
        verbose = 1;
    return 0;
}
