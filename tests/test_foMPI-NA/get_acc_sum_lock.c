#include <assert.h>

#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_get_acc_sum_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int flag, create_flavor;

  int* recv_buf;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  recv_buf = malloc( DIM * sizeof(int) );

  status = foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  for( i=0 ; i<DIM ; i++ ) {
    base[i] = i;
  }
  if (rank == 0) {
    for( i=0 ; i<DIM ; i++ ) {
      base[DIM+i] = i;
    }
  }

  status = foMPI_Win_unlock( rank, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, &create_flavor, &flag );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  assert( flag != 0 );
  
  if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) {
    if ( rank == 0 ) {
      MPI_Get_address( &base[DIM], &start);
    }
    MPI_Bcast( &start, 1, MPI_AINT, 0, comm ); /* ends the init phase */
  } else {
    start = DIM;
    MPI_Barrier( comm ); /* ends the init phase */
  }

  status = foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  status = foMPI_Get_accumulate( &base[0], DIM, MPI_INT, &recv_buf[0], DIM, MPI_INT, 0, start, DIM, MPI_INT, foMPI_SUM, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_unlock( 0, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Barrier( comm ); /* wait for all accesses to finish */

  if ( rank == 0 ) {
    for( i=0 ; i<DIM ; i++ ) {
      if( base[DIM+i] != ((commsize+1)*i) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, i, base[DIM+i], (commsize+1)*i );
        exit(EXIT_FAILURE);
      }
    }
  }
  if( recv_buf[0] != 0) {
    fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 0, recv_buf[0], 0 );
  exit(EXIT_FAILURE);
  }
  for( i=1 ; i<DIM ; i++ ) {
    if( (recv_buf[i] % i) != 0) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i (%i) is not a multiple of %i.\n", rank, __FILE__, __LINE__, i, recv_buf[i], i );
      exit(EXIT_FAILURE);
    }
  }

  if (rank == 0) {
    printf("No Error\n");
  }

  return;
}
