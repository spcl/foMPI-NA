/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"
#include "mpitypes.h"
#include "mpitypes-config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#define COUNT		4
#define SIZE		340
#define EL_COUNT	1131

static int verbose = 0;

/* helper functions */
static int parse_args(int argc, char **argv);

int main( int argc, char **argv )
{
    int 		rank, size, ret, errs = 0; 
    MPI_Datatype 	struct_type, type1[COUNT];
    MPI_Aint 		disp1[COUNT] = {0, 0, 332, 340};
    int			block1[COUNT] = {1, 56, 2, 1};

    char s_buf[EL_COUNT*SIZE];
    char r_buf[EL_COUNT*SIZE];

    MPI_Init(&argc, &argv);
    parse_args(argc, argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    type1[0] = MPI_LB;
    type1[1] = MPI_FLOAT;
    type1[2] = MPI_FLOAT;
    type1[3] = MPI_UB;
    
    ret = MPI_Type_struct(4, block1, disp1, type1, &struct_type);
    if (ret != MPI_SUCCESS) 
    {
	errs++;
        if (verbose) fprintf(stderr, "Could not make struct type.\n");
    }
    
    ret = MPI_Type_commit(&struct_type);
    if (ret != MPI_SUCCESS) 
    {
	errs++;
        if (verbose) fprintf(stderr, "Could not commit struct type.\n");
    }
    
    ret = MPIT_Type_init(struct_type);
    if (ret != MPI_SUCCESS)
    {
	errs++;
	if (verbose) fprintf(stderr, "Error in MPIT_Type_init.\n");
    }

    memset(s_buf, 0, EL_COUNT*SIZE);
    memset(r_buf, 0, EL_COUNT*SIZE);

#if 0
    MPI_Isend(s_buf, EL_COUNT, struct_type, 0, 4, MPI_COMM_WORLD, &request);
    MPI_Recv(r_buf, EL_COUNT, struct_type, 0, 4, MPI_COMM_WORLD, &Status );
    MPI_Wait(&request, &Status);
#endif
    
    MPI_Type_free(&struct_type);
    
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
