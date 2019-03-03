/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *
 *  (C) 2003 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpitest.h"

/*
static char MTEST_Descrip[] = "RMA to self";
*/

int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, i, j;
    MPI_Comm      comm;
    foMPI_Win       win;
    int           *winbuf, count;
    int           *sbuf, scount, vcount;
    MPI_Datatype  vectype;

    foMPI_Init( &argc, &argv );
    MTest_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;

    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );

    /* Allocate and initialize sbuf */
    scount = 1000;
    count  = 1000;
    sbuf   = (int *)malloc( scount * sizeof(int) );
    if (!sbuf) {
	fprintf( stderr, "Could not allocate send buffer f size %d\n", 
		 scount );
	MPI_Abort( MPI_COMM_WORLD, 0 );
    }
    for (i=0; i<scount; i++) sbuf[i] = i;

    MPI_Alloc_mem( count*sizeof(int), MPI_INFO_NULL, &winbuf );

    /* This is a simple vector type */
    vcount = count / 4;
    MPI_Type_vector( vcount, 1, 2, MPI_INT, &vectype );
    MPI_Type_commit( &vectype );
    foMPI_Win_create( winbuf, count * sizeof(int), sizeof(int), MPI_INFO_NULL, 
		    comm, &win );

    /* Check with different combination of types, including non-contig on 
       both sides */
    
    /* Clear winbuf */
    memset( winbuf, 0, count*sizeof(int) );
    foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
    foMPI_Put( sbuf, 1, vectype, rank, 0, 1, vectype, win );
    foMPI_Win_unlock( rank, win );
    /* Check results */
    j = 0;
    for (i=0; i<vcount; i++) {
	if (winbuf[j] != sbuf[j]) {
	    errs ++;
	    fprintf( stderr, "VecPut: winbuf[%d] = %d, should = %d\n", 
		     j, winbuf[j], sbuf[j] );
	}
	j += 2;
    }

    memset( winbuf, 0, count*sizeof(int) );
    foMPI_Win_lock( foMPI_LOCK_SHARED, rank, 0, win );
    foMPI_Accumulate( sbuf, 1, vectype, rank, 0, 1, vectype, foMPI_SUM, win );
    foMPI_Win_unlock( rank, win );
    /* Check results */
    j = 0;
    for (i=0; i<vcount; i++) {
	if (winbuf[j] != sbuf[j]) {
	    errs ++;
	    fprintf( stderr, "VecAcc: winbuf[%d] = %d, should = %d\n", 
		     j, winbuf[j], sbuf[j] );
	}
	j += 2;
    }

    /* Now, use get to fetch back the results that we just wrote */
    memset( sbuf, 0, count*sizeof(int) );
    foMPI_Win_lock( foMPI_LOCK_SHARED, rank, 0, win );
    foMPI_Get( sbuf, 1, vectype, rank, 0, 1, vectype, win );
    foMPI_Win_unlock( rank, win );
    /* Check results */
    j = 0;
    for (i=0; i<vcount; i++) {
	if (winbuf[j] != sbuf[j]) {
	    errs ++;
	    fprintf( stderr, "VecGet: winbuf[%d] = %d, should = %d\n", 
		     j, winbuf[j], sbuf[j] );
	}
	j += 2;
    }

    foMPI_Win_free( &win );
    MPI_Free_mem( winbuf );
    free( sbuf );
    foMPI_Type_free( &vectype );

    MTest_Finalize( errs );

    foMPI_Finalize();
    return 0;
}