#include <assert.h>
#include "fompi.h"
#include "test_mpi3rma.h"

int main( int argc, char *argv[] ) {

  foMPI_Win win;
  int status;

  uint64_t* base;
  uint64_t result;

  int commsize;
  int commrank;

  int ranges[1][3];
  MPI_Group group, newgroup;

  status = foMPI_Init( &argc, &argv );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Win_allocate( 2*sizeof(uint64_t), sizeof(uint64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &base, &win);
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  MPI_Comm_size( MPI_COMM_WORLD, &commsize );
  MPI_Comm_rank( MPI_COMM_WORLD, &commrank );

  assert( commsize == 2 );

  base[0] = commrank;
  base[1] = commrank;

  foMPI_Win_fence( 0, win );

  ranges[0][0] = (commrank+1) % 2;
  ranges[0][1] = commsize-1;
  ranges[0][2] = 2;
  MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Group_range_incl(group, 1, ranges, &newgroup);
  MPI_Group_free( &group );

  foMPI_Win_post(newgroup, 0, win);
  foMPI_Win_start(newgroup, 0, win);
  foMPI_Win_complete(win);
  foMPI_Win_wait(win);

  MPI_Group_free( &newgroup );

  MPI_Barrier( MPI_COMM_WORLD ); //sync change

  if ( commrank == 0 ) {
    foMPI_Win_lock( foMPI_LOCK_SHARED, 1, 0, win );
    foMPI_Put( &base[0], 1, MPI_UINT64_T, 1, 0, 1, MPI_UINT64_T, win );
    foMPI_Get( &base[1], 1, MPI_UINT64_T, 1, 1, 1, MPI_UINT64_T, win );
    foMPI_Win_flush( 1, win );
    foMPI_Win_unlock( 1, win );

    foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, 1, 0, win );
    foMPI_Compare_and_swap( &base[0], &base[1], &result, MPI_UINT64_T, 1, 1, win );    
    foMPI_Win_unlock( 1, win );

    foMPI_Win_lock_all( 0, win );
    foMPI_Fetch_and_op( &base[1], &result, MPI_UINT64_T, 1, 1, foMPI_SUM, win );
    foMPI_Accumulate( &base[0], 1, MPI_UINT64_T, 1, 1, 1, MPI_UINT64_T, foMPI_BOR, win );
    foMPI_Get_accumulate( &base[1], 1, MPI_UINT64_T, &result, 1, MPI_UINT64_T, 1, 1, 1, MPI_UINT64_T, foMPI_SUM, win );
    foMPI_Win_flush_all( win );
    foMPI_Win_unlock_all( win );
  }
  MPI_Barrier( MPI_COMM_WORLD );
  //assert( base[0] == 0 );
  assert( base[1] == 1 );

  status = foMPI_Win_free( &win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Finalize();
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  return 0;
}
