// Copyright (c) 2014 ETH-Zurich. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fompi.h"

int main( int argc, char *argv[] ) {

  foMPI_Win win;

  uint64_t* base;

  int commsize;
  int commrank;

  foMPI_Init( &argc, &argv );

  foMPI_Win_allocate( 2*sizeof(uint64_t), sizeof(uint64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &base, &win);

  MPI_Comm_size( MPI_COMM_WORLD, &commsize );
  MPI_Comm_rank( MPI_COMM_WORLD, &commrank );

  base[0] = 1;
  base[1] = 0;

  foMPI_Win_fence( 0 /* assert */, win );

  foMPI_Accumulate( &base[0], 1, MPI_UINT64_T, 0, 1, 1, MPI_UINT64_T, foMPI_SUM, win );
  
  foMPI_Win_fence( 0 /* assert */, win );

  if ( commrank == 0 ) {
    if ( base[1] == commsize ) {
      printf("Reached expected sum of commsize\nThank you for using foMPI.\n");
    } else {
      printf("Accumulated %lu instead of %i.\n", base[1], commsize);
    }
  }

  foMPI_Win_free( &win );

  foMPI_Finalize();

  return 0;
}
