#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_4( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  int local_DIM = 5;

  MPI_Datatype target_dtype;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  if (rank == 0) {
    for( i=0 ; i<3 ; i++ ) {
      base[i] = 0;
    }
    for( i=0 ; i<4 ; i++ ) {
      base[2*local_DIM+i] = 0;
    }
  }
  for( i=0 ; i<7 ; i++ ) {
    base[3+i] = rank+i;
  }
 
  int blocklength[2] = {3, 4};
  int displacement[2] = { 0, 10 };

  MPI_Type_indexed(2, &blocklength[0], &displacement[0], MPI_INT, &target_dtype);
  MPI_Type_commit(&target_dtype);

  MPI_Get_address( &base[0], &start );
  MPI_Bcast( &start, 1, MPI_AINT, 0, comm );

  status = foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Accumulate( &base[3], 7, MPI_INT, 0, start, 1, target_dtype, foMPI_SUM, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
 
  status = foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &target_dtype );

  if ( rank == 0 ) {
    for( k=0 ; k<3 ; k++ ) {
      if( base[k] != ((commsize-1)*commsize/2+k*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, k, base[k], (commsize-1)*commsize/2+k*commsize );
        exit(EXIT_FAILURE);
      }
    }
    for( k=0 ; k<4 ; k++ ) {
      if( base[2*local_DIM+k] != ((commsize-1)*commsize/2+(3+k)*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 2*local_DIM+k, base[2*local_DIM+k], (commsize-1)*commsize/2+(3+k)*commsize );
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
