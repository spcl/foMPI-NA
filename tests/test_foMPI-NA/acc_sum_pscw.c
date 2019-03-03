#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_acc_sum_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int ranges[1][3];
  MPI_Group group;
  MPI_Group newgroup;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );
  
  if ( rank == 0 ) {
    ranges[0][0] = 1;
    ranges[0][1] = commsize-1;
    ranges[0][2] = 1;
  } else {
    ranges[0][0] = 0;
    ranges[0][1] = 0;
    ranges[0][2] = 1;
  }

  MPI_Comm_group(comm, &group);
  MPI_Group_range_incl(group, 1, ranges, &newgroup);
  MPI_Group_free( &group );

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if ( rank == 0 ) {
      MPI_Get_address( &base[0], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm );
  } else {
    start = 0;
  }

  for( i=0 ; i<DIM ; i++ ) {
    base[i] = rank+i;
  }

  if( rank == 0 ) {
    status = foMPI_Win_post(newgroup, 0, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    status = foMPI_Win_wait(win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  } else {
    status = foMPI_Win_start(newgroup, 0, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    status = foMPI_Accumulate( &base[0], DIM, MPI_INT, 0, start, DIM, MPI_INT, foMPI_SUM, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    status = foMPI_Win_complete(win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  MPI_Group_free( &newgroup );

  if ( rank == 0 ) {
    for( i=0 ; i<DIM ; i++ ) {
      if( base[i] != ((commsize-1)*((commsize-1)+1)/2+i*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], (commsize-1)*((commsize-1)+1)/2+i*commsize );
        exit(EXIT_FAILURE);
      }
    }
  }

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
