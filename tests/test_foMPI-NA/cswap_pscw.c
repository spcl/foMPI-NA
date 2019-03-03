#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_cswap_pscw( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;
  
  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */
  int comparand;
  int ranges[1][3];
  MPI_Group group;
  MPI_Group newgroup;
  int* result;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  base[0] = rank % 2;

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
    if (rank == 0) {
      base_adr = malloc( commsize * sizeof(MPI_Aint) );
    }
    MPI_Get_address( &base[0], &start);
    MPI_Gather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, 0, comm );
  } else {
    start = 0;
  }

  if (rank == 0) {
    result = malloc( commsize * sizeof(int) );

    status = foMPI_Win_start(newgroup, 0, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    comparand = 0;
    base[2] = 1;
    for( i = 1 ; i<commsize ; i++ ) {
      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
        status = foMPI_Compare_and_swap( &base[2],  &comparand, &result[i], MPI_INT, i, base_adr[i], win);
      } else {
        status = foMPI_Compare_and_swap( &base[2],  &comparand, &result[i], MPI_INT, i, start, win);
      }
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
    
    status = foMPI_Win_complete(win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    for( i = 1 ; i<commsize ; i++ ) {
      if ( result[i] != (i % 2) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: MPI_Compare_swap error - result %i doesn't match %i\n", rank, __FILE__, __LINE__, result[i], (i % 2) );
        exit(EXIT_FAILURE);
      }
    }
    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      free( base_adr );
    }
    free( result );
  } else {
    status = foMPI_Win_post(newgroup, 0, win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    
    status = foMPI_Win_wait(win);
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
    if ( base[0] != 1 ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 0, base[0], 1 );
      exit(EXIT_FAILURE);
    }

  }

  MPI_Group_free( &newgroup );

  MPI_Barrier( comm );
 
  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
