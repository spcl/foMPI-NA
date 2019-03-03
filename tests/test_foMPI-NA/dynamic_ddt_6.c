#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_6( int* base, int DIM, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  MPI_Datatype target_dtype;

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  for( i=0 ; i<DIM ; i++ ) {
    base[2+i] = rank+i;
  }
  if (rank == 0) { 
    for( i=0 ; i<DIM ; i++ ) {
      base[7+i] = 0;
    }
  }

  MPI_Type_contiguous( DIM, MPI_INT, &target_dtype);
  MPI_Type_commit( &target_dtype );

  MPI_Get_address( &base[DIM+2], &start );
  MPI_Bcast( &start, 1, MPI_AINT, 0, comm );

  status = foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Accumulate( &base[2], DIM, MPI_INT, 0, start, 1, target_dtype, foMPI_SUM, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
  
  status = foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &target_dtype );

  if ( rank == 0 ) {
    for( i=0 ; i<5 ; i++ ) {
      if( base[DIM+2+i] != ((commsize-1)*commsize/2+i*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i. commsize = %i\n", rank, __FILE__, __LINE__, DIM+2+i, base[DIM+2+i], (commsize-1)*commsize/2+i*commsize, commsize );
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}
