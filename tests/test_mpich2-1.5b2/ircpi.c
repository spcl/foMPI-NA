/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h"
#include "mpi.h" 
#include "stdio.h"
#include <math.h> 

/* From Using MPI-2 */

int main(int argc, char *argv[]) 
{ 
    int n, myid, numprocs, i, ierr; 
    double PI25DT = 3.141592653589793238462643; 
    double mypi, pi, h, sum, x; 
    foMPI_Win nwin, piwin; 
 
    foMPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); 
 
    if (myid == 0) { 
	foMPI_Win_create(&n, sizeof(int), 1, MPI_INFO_NULL, 
		       MPI_COMM_WORLD, &nwin); 
	foMPI_Win_create(&pi, sizeof(double), 1, MPI_INFO_NULL, 
		       MPI_COMM_WORLD, &piwin);  
    } 
    else { 
	foMPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL, 
		       MPI_COMM_WORLD, &nwin); 
	foMPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL, 
		       MPI_COMM_WORLD, &piwin); 
    } 
    while (1) { 
        if (myid == 0) { 
            fprintf(stdout, "Enter the number of intervals: (0 quits) ");
	    fflush(stdout); 
            ierr=scanf("%d",&n); 
	    pi = 0.0;			 
        } 
	foMPI_Win_fence(0, nwin); 
	if (myid != 0)  
	    foMPI_Get(&n, 1, MPI_INT, 0, 0, 1, MPI_INT, nwin); 
	foMPI_Win_fence(0, nwin); 
        if (n == 0) 
            break; 
        else { 
            h   = 1.0 / (double) n; 
            sum = 0.0; 
            for (i = myid + 1; i <= n; i += numprocs) { 
                x = h * ((double)i - 0.5); 
                sum += (4.0 / (1.0 + x*x)); 
            } 
            mypi = h * sum; 
	    foMPI_Win_fence( 0, piwin); 
	    foMPI_Accumulate(&mypi, 1, MPI_DOUBLE, 0, 0, 1, MPI_DOUBLE, 
			   foMPI_SUM, piwin); 
	    foMPI_Win_fence(0, piwin); 
            if (myid == 0) { 
                fprintf(stdout, "pi is approximately %.16f, Error is %.16f\n", 
                       pi, fabs(pi - PI25DT)); 
		fflush(stdout);
	    }
        } 
    } 
    foMPI_Win_free(&nwin); 
    foMPI_Win_free(&piwin); 
    foMPI_Finalize(); 
    return 0; 
} 
