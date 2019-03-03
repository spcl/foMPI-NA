/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * Note: This version of the test uses an approach of creating a flattened
 *       representation of the type and manually copying pieces inside
 *       pack_and_upack(), testing the blockct and flatten functionalities.
 */
#include "mpi.h"
#include "mpitypes.h"
#include "mpitypes-config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

static int verbose = 0;

/* tests */
int darray_2d_c_test1(void);
int darray_4d_c_test1(void);

/* helper functions */
static int parse_args(int argc, char **argv);
static int pack_and_unpack(char *typebuf,
			   int count,
			   MPI_Datatype datatype,
			   int typebufsz);

int main(int argc, char **argv)
{
    int err, errs = 0;

    MPI_Init( &argc, &argv );
    parse_args(argc, argv);


    /* perform some tests */
    err = darray_2d_c_test1();
    if (err && verbose) fprintf(stderr,
				"%d errors in 2d darray c test 1.\n", err);
    errs += err;

    err = darray_4d_c_test1();
    if (err && verbose) fprintf(stderr,
				"%d errors in 4d darray c test 1.\n", err);
    errs += err;

    /* print message and exit */
    if (errs) {
	fprintf(stderr, "Found %d errors\n", errs);
    }

    MPI_Finalize();
    return errs ? 1 : 0;
}

/* darray_2d_test1()
 *
 * Performs a sequence of tests building darrays with single-element
 * blocks, running through all the various positions that the element might
 * come from.
 *
 * Returns the number of errors encountered.
 */
int darray_2d_c_test1(void)
{
    MPI_Datatype darray;
    int array[9]; /* initialized below */
    int array_size[2] = {3, 3};
    int array_distrib[2] = {MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK};
    int array_dargs[2] = {MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG};
    int array_psizes[2] = {3, 3};

    int i, rank, err, errs = 0, sizeoftype;

    /* pretend we are each rank, one at a time */
    for (rank=0; rank < 9; rank++) {
	/* set up buffer */
	for (i=0; i < 9; i++) {
	    array[i] = i;
	}

	/* set up type */
	err = MPI_Type_create_darray(9, /* size */
				     rank,
				     2, /* dims */
				     array_size,
				     array_distrib,
				     array_dargs,
				     array_psizes,
				     MPI_ORDER_C,
				     MPI_INT,
				     &darray);
	if (err != MPI_SUCCESS) {
	    errs++;
	    if (verbose) {
		fprintf(stderr,
			"error in MPI_Type_create_darray call; aborting after %d errors\n",
			errs);
	    }
	    return errs;
	}
	
	MPI_Type_commit(&darray);

	MPI_Type_size(darray, &sizeoftype);
	if (sizeoftype != sizeof(int)) {
	    errs++;
	    if (verbose) fprintf(stderr, "size of type = %d; should be %d\n",
				 sizeoftype, (int) sizeof(int));
	    return errs;
	}
	
	err = pack_and_unpack((char *) array, 1, darray, 9*sizeof(int));
	
	for (i=0; i < 9; i++) {

	    if ((i == rank) && (array[i] != rank)) {
		errs++;
		if (verbose) fprintf(stderr, "array[%d] = %d; should be %d\n",
				     i, array[i], rank);
	    }
	    else if ((i != rank) && (array[i] != 0)) {
		errs++;
		if (verbose) fprintf(stderr, "array[%d] = %d; should be %d\n",
				     i, array[i], 0);
	    }
	}
	MPI_Type_free(&darray);
    }

    return errs;
}

/* darray_4d_c_test1()
 *
 * Returns the number of errors encountered.
 */
int darray_4d_c_test1(void)
{
    MPI_Datatype darray;
    int array[72];
    int array_size[4] = {6, 3, 2, 2};
    int array_distrib[4] = { MPI_DISTRIBUTE_BLOCK,
			     MPI_DISTRIBUTE_BLOCK,
			     MPI_DISTRIBUTE_NONE,
			     MPI_DISTRIBUTE_NONE };
    int array_dargs[4] = { MPI_DISTRIBUTE_DFLT_DARG,
			   MPI_DISTRIBUTE_DFLT_DARG,
			   MPI_DISTRIBUTE_DFLT_DARG,
			   MPI_DISTRIBUTE_DFLT_DARG };
    int array_psizes[4] = {6, 3, 1, 1};

    int i, rank, err, errs = 0, sizeoftype;

    for (rank=0; rank < 18; rank++) {
	/* set up array */
	for (i=0; i < 72; i++) {
	    array[i] = i;
	}

	/* set up type */
	err = MPI_Type_create_darray(18, /* size */
				     rank,
				     4, /* dims */
				     array_size,
				     array_distrib,
				     array_dargs,
				     array_psizes,
				     MPI_ORDER_C,
				     MPI_INT,
				     &darray);
	if (err != MPI_SUCCESS) {
	    errs++;
	    if (verbose) {
		fprintf(stderr,
			"error in MPI_Type_create_darray call; aborting after %d errors\n",
			errs);
	    }
	    return errs;
	}

	MPI_Type_commit(&darray);

	/* verify the size of the type */
	MPI_Type_size(darray, &sizeoftype);
	if (sizeoftype != 4*sizeof(int)) {
	    errs++;
	    if (verbose) fprintf(stderr, "size of type = %d; should be %d\n",
				 sizeoftype, (int) (4*sizeof(int)));
	    return errs;
	}

	/* pack and unpack the type, zero'ing out all other values */
	err = pack_and_unpack((char *) array, 1, darray, 72*sizeof(int));

	for (i=0; i < 4*rank; i++) {
	    if (array[i] != 0) {
		errs++;
		if (verbose) fprintf(stderr, "array[%d] = %d; should be %d\n",
				     i, array[i], 0);
	    }
	}

	for (i=4*rank; i < 4*rank + 4; i++) {
	    if (array[i] != i) {
		errs++;
		if (verbose) fprintf(stderr, "array[%d] = %d; should be %d\n",
				     i, array[i], i);
	    }
	}
	for (i=4*rank+4; i < 72; i++) {
	    if (array[i] != 0) {
		errs++;
		if (verbose) fprintf(stderr, "array[%d] = %d; should be %d\n",
				     i, array[i], 0);
	    }
	}

	MPI_Type_free(&darray);
    }
    return errs;
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


static int parse_args(int argc, char **argv)
{
    /*
    int ret;

    while ((ret = getopt(argc, argv, "v")) >= 0)
    {
	switch (ret) {
	    case 'v':
		verbose = 1;
		break;
	}
    }
    */
    if (argc > 1 && strcmp(argv[1], "-v") == 0)
	verbose = 1;
    return 0;
}
