#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_get_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int ranges[1][3];
  MPI_Group group;
  MPI_Group newgroup;
  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  ranges[0][0] = (rank+1) % 2;
  ranges[0][1] = commsize-1;
  ranges[0][2] = 2;
  MPI_Comm_group(comm, &group);
  MPI_Group_range_incl(group, 1, ranges, &newgroup);
  MPI_Group_free( &group );

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    base_adr = malloc( commsize * sizeof(MPI_Aint) );
    MPI_Get_address( &base[2*DIM], &start );
    MPI_Allgather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, comm );
  } else {
    start = 2*DIM;
  }

  for (i=0 ; i<DIM ; i++ ) {
    base[i] = 0;
    base[2*DIM+i] = 4*DIM+i;
  }

  status = foMPI_Win_post(newgroup, 0, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  status = foMPI_Win_start(newgroup, 0, win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  for ( i=((rank+1)%2) ; i<commsize ; i+=2 ) {
    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      status = foMPI_Get( &base[0], DIM, MPI_INT, i, base_adr[i], DIM, MPI_INT, win );
    } else {
      status = foMPI_Get( &base[0], DIM, MPI_INT, i, start, DIM, MPI_INT, win );
    }
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }
  
  status = foMPI_Win_complete(win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  status = foMPI_Win_wait(win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    free( base_adr );
  }
  MPI_Group_free( &newgroup );

  for( i=0 ; i<DIM ; i++) {
    if( base[i] != 4*DIM+i ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[i], 4*DIM+i);
      exit(EXIT_FAILURE);
    }
  }

  MPI_Barrier( comm );

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
