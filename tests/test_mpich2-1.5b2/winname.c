/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "fompi.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "mpitestconf.h"
#include "mpitest.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif

int main( int argc, char *argv[] )
{
    int errs = 0;
    foMPI_Win win;
    int cnt, namelen;
    char name[MPI_MAX_OBJECT_NAME], nameout[MPI_MAX_OBJECT_NAME];

    foMPI_Init( &argc, &argv );
    MTest_Init( &argc, &argv );

    cnt = 0;
    while (MTestGetWin( &win, 1 )) {
	if (win == MPI_WIN_NULL) continue;
    
	sprintf( name, "win-%d", cnt );
	cnt++;
	foMPI_Win_set_name( win, name );
	nameout[0] = 0;
	foMPI_Win_get_name( win, nameout, &namelen );
	if (strcmp( name, nameout )) {
	    errs++;
	    printf( "Unexpected name, was %s but should be %s\n",
		    nameout, name );
	}

	MTestFreeWin( &win );
    }

    MTest_Finalize( errs );
    foMPI_Finalize();
    return 0;
}
