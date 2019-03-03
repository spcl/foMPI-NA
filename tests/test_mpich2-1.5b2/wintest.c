/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "fompi.h" 
#include "mpi.h"
#include "stdio.h"
#include "mpitest.h"

/* tests put and get with post/start/complete/test on 2 processes */
/* Same as test2.c, but uses win_test instead of win_wait */

#define SIZE1 10
#define SIZE2 20

int main(int argc, char *argv[]) 
{ 
    int rank, destrank, nprocs, A[SIZE2], B[SIZE2], i;
    MPI_Comm CommDeuce;
    MPI_Group comm_group, group;
    foMPI_Win win;
    int errs = 0, flag;

    foMPI_Init(&argc,&argv);
    MTest_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs); 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank); 

    if (nprocs < 2) {
        printf("Run this program with 2 or more processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_split(MPI_COMM_WORLD, (rank < 2), rank, &CommDeuce);

    if (rank < 2) {
        MPI_Comm_group(CommDeuce, &comm_group);

        if (rank == 0) {
            for (i=0; i<SIZE2; i++) A[i] = B[i] = i;
            foMPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, CommDeuce, &win);
            destrank = 1;
            MPI_Group_incl(comm_group, 1, &destrank, &group);
            foMPI_Win_start(group, 0, win);
            for (i=0; i<SIZE1; i++)
                foMPI_Put(A+i, 1, MPI_INT, 1, i, 1, MPI_INT, win); 
            for (i=0; i<SIZE1; i++)
                foMPI_Get(B+i, 1, MPI_INT, 1, SIZE1+i, 1, MPI_INT, win);

            foMPI_Win_complete(win);

            for (i=0; i<SIZE1; i++) 
                if (B[i] != (-4)*(i+SIZE1)) {
                    printf("Get Error: B[i] is %d, should be %d\n", B[i], (-4)*(i+SIZE1));
                    errs++;
                }
        }
        else {  /* rank=1 */
            for (i=0; i<SIZE2; i++) B[i] = (-4)*i;
            foMPI_Win_create(B, SIZE2*sizeof(int), sizeof(int), MPI_INFO_NULL, CommDeuce, &win);
            destrank = 0;
            MPI_Group_incl(comm_group, 1, &destrank, &group);
            foMPI_Win_post(group, 0, win);
            flag = 0;
            while (!flag)
                foMPI_Win_test(win, &flag);

            for (i=0; i<SIZE1; i++) {
                if (B[i] != i) {
                    printf("Put Error: B[i] is %d, should be %d\n", B[i], i);
                    errs++;
                }
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
