/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h"
#include "mpi.h" 
#include "stdio.h"
#include "mpitest.h"

/* transposes a matrix using post/start/complete/wait and derived
   datatypes. Uses  vector and hvector (Example 3.32 from MPI 1.1
   Standard). Run on 2 processes */

#define NROWS 100
#define NCOLS 100

int main(int argc, char *argv[]) 
{ 
    int rank, nprocs, A[NROWS][NCOLS], i, j, destrank;
    MPI_Comm CommDeuce;
    foMPI_Win win;
    MPI_Datatype column, xpose;
    MPI_Group comm_group, group;
    int errs=0;

    foMPI_Init(&argc,&argv);
    MTest_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs); 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank); 

    if (nprocs < 2) {
        printf("Run this program with 2 or more processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_split(MPI_COMM_WORLD, (rank < 2), rank, &CommDeuce);

    if (rank < 2)
    {
        MPI_Comm_group(CommDeuce, &comm_group);

        if (rank == 0)
        {
            for (i=0; i<NROWS; i++)
                for (j=0; j<NCOLS; j++)
                    A[i][j] = i*NCOLS + j;

            /* create datatype for one column */
            MPI_Type_vector(NROWS, 1, NCOLS, MPI_INT, &column);
            /* create datatype for matrix in column-major order */
            MPI_Type_hvector(NCOLS, 1, sizeof(int), column, &xpose);
            MPI_Type_commit(&xpose);

            foMPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, CommDeuce, &win);

            destrank = 1;
            MPI_Group_incl(comm_group, 1, &destrank, &group);
            foMPI_Win_start(group, 0, win);

            foMPI_Put(A, NROWS*NCOLS, MPI_INT, 1, 0, 1, xpose, win);

            foMPI_Type_free(&column);
            foMPI_Type_free(&xpose);

            foMPI_Win_complete(win);
        }
        else
        { /* rank=1 */
            for (i=0; i<NROWS; i++)
                for (j=0; j<NCOLS; j++)
                    A[i][j] = -1;
            foMPI_Win_create(A, NROWS*NCOLS*sizeof(int), sizeof(int), MPI_INFO_NULL, CommDeuce, &win);
            destrank = 0;
            MPI_Group_incl(comm_group, 1, &destrank, &group);
            foMPI_Win_post(group, 0, win);
            foMPI_Win_wait(win);

            for (j=0; j<NCOLS; j++)
            {
                for (i=0; i<NROWS; i++)
                {
                    if (A[j][i] != i*NCOLS + j)
                    {
                        if (errs < 50)
                        {
                            printf("Error: A[%d][%d]=%d should be %d\n", j, i,
                                   A[j][i], i*NCOLS + j);
                        }
                        errs++;
                    }
                }
            }
            if (errs >= 50)
            {
                printf("Total number of errors: %d\n", errs);
            }
        }

        MPI_Group_free(&group);
        MPI_Group_free(&comm_group);
        foMPI_Win_free(&win);
    }
    MPI_Comm_free(&CommDeuce);
    MTest_Finalize(errs);
    foMPI_Finalize(); 
    return 0; 
} 
