#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_5( int* base, foMPI_Win win, MPI_Comm comm ) {

  int i;
  int k;
  int rank;
  int commsize;
  int status;
  MPI_Aint start;

  MPI_Datatype target_dtype;
  MPI_Datatype origin_dtype;
  int blocklength[3];
  int displacement[3];

  MPI_Comm_rank( comm, &rank );
  MPI_Comm_size( comm, &commsize );

  if (rank == 0) {
    for( i=3 ; i<6 ; i++ ) {
      base[i] = 0;
    }
    base[8] = 0;
    for( i=11 ; i<14 ; i++ ) {
      base[i] = 0;
    }
  }
  for( i=0 ; i<3 ; i++ ) {
    base[i] = rank+i;
  }
  for( i=0 ; i<2 ; i++ ) {
    base[6+i] = rank+i+3;
  }
  for( i=0 ; i<2 ; i++ ) {
    base[9+i] = rank+i+5;
  }
 
  blocklength[0] = 3;
  blocklength[1] = 1;
  blocklength[2] = 3;
  displacement[0] = 3;
  displacement[1] = 8;
  displacement[2] = 11;

  MPI_Type_indexed(3, &blocklength[0], &displacement[0], MPI_INT, &target_dtype);
  MPI_Type_commit(&target_dtype);

  blocklength[0] = 3;
  blocklength[1] = 2;
  blocklength[2] = 2;
  displacement[0] = 0;
  displacement[1] = 6;
  displacement[2] = 9;

  MPI_Type_indexed(3, &blocklength[0], &displacement[0], MPI_INT, &origin_dtype);
  MPI_Type_commit(&origin_dtype);

  MPI_Get_address( &base[0], &start );
  MPI_Bcast( &start, 1, MPI_AINT, 0, comm );

  status = foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  status = foMPI_Accumulate( &base[0], 1, origin_dtype, 0, start, 1, target_dtype, foMPI_SUM, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
 
  status = foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win );
  _check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

  foMPI_Type_free( &target_dtype );
  foMPI_Type_free( &origin_dtype );

  if ( rank == 0 ) {
    for( k=0 ; k<3 ; k++ ) {
      if( base[3+k] != ((commsize-1)*commsize/2+k*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 3+k, base[3+k], (commsize-1)*commsize/2+k*commsize );
        exit(EXIT_FAILURE);
      }
    }
    if( base[8] != ((commsize-1)*commsize/2+3*commsize) ) {
      fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 8, base[8], (commsize-1)*commsize/2+3*commsize );
      exit(EXIT_FAILURE);
    }
    for( k=0 ; k<3 ; k++ ) {
      if( base[11+k] != ((commsize-1)*commsize/2+(4+k)*commsize) ) {
        fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank, __FILE__, __LINE__, 11+k, base[11+k], (commsize-1)*commsize/2+(4+k)*commsize );
        exit(EXIT_FAILURE);
      }
    }
  }

  if ( rank == 0 ) {
    printf("No Error\n");
  }

  return;
}

