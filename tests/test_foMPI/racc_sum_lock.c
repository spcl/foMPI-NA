#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_racc_sum_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int create_flavor;

  foMPI_Request* request;
  int flag;
  MPI_Aint* base_adr = NULL; /* to prevent compiler warnings */

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  for( i=0 ; i<DIM ; i++ ) {
    base[i] = rank+i;
    base[DIM+i] = 0;
  }

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    base_adr = malloc( commsize * sizeof(MPI_Aint) );
    MPI_Get_address( &base[DIM], &start);
    MPI_Allgather( &start, 1, MPI_AINT, &base_adr[0], 1, MPI_AINT, comm ); /* finishes the init phase */
  } else {
    start = DIM;
    MPI_Barrier( comm ); /* finishes the init phase */
  }

  request = malloc( commsize * sizeof(foMPI_Request) );

  for ( i=0 ; i<commsize ; i++ ) {
    status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, 0, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }
  
  for ( i=0 ; i<commsize ; i++ ) {
    if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
      status = foMPI_Raccumulate( &base[0], DIM, MPI_INT, i, base_adr[i], DIM, MPI_INT, foMPI_SUM, win, &request[i]);
    } else {
      status = foMPI_Raccumulate( &base[0], DIM, MPI_INT, i, start, DIM, MPI_INT, foMPI_SUM, win, &request[i]);
    }
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }

  for( i=commsize-1 ; i>=0 ; i-- ) {
    flag = 2;
    status = foMPI_Test( &request[i], &flag, MPI_STATUS_IGNORE );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    if (flag == 1) {
      //printf("%i: finished %i\n", debug_pe, i);
    }
  }
  for ( i=0 ; i<commsize ; i++ ) {
    status = foMPI_Win_unlock( i, win );
    _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  }
  for( i=0 ; i<commsize ; i++ ) {
    if ( request[i] != foMPI_REQUEST_NULL ) {
      status = foMPI_Wait( &request[i], MPI_STATUS_IGNORE );
      _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
    }
  }

  MPI_Barrier( comm ); /* wait for end of all accesses */

  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    free( base_adr );
  }

  free( request );

  for( i=0 ; i<DIM ; i++ ) {
    if( base[DIM+i] != ((commsize-1)*((commsize-1)+1)/2+i*commsize) ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[DIM+i], (commsize-1)*((commsize-1)+1)/2+i*commsize );
      exit(EXIT_FAILURE);
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
