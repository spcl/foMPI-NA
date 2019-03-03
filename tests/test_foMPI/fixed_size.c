#include "fompi.h"

/* gerro: for testing purpose, I used the following configuration: fixed size = 24 bytes, and 3 buffer */

int main( int argc, char *argv[] ) {

  foMPI_Win win;

  uint64_t* buf;

  int commsize;
  int commrank;

  int base;
  int i;

  int blk_length[4];
  int blk_displ[7];

  MPI_Datatype origin_datatype, target_datatype, temp_datatype;

  foMPI_Init( &argc, &argv );

  foMPI_Win_allocate( 16*sizeof(uint64_t), sizeof(uint64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &buf, &win);

  MPI_Comm_size( MPI_COMM_WORLD, &commsize );
  MPI_Comm_rank( MPI_COMM_WORLD, &commrank );

  /* init for first test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* first test: contiguous with datatype intrinisics which are in foMPI's fast path */
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 5, MPI_UINT64_T, 1 /* rank */, 5 /* offset */, 5 /* count */, MPI_UINT64_T, win );
  }

  foMPI_Win_fence( 0, win );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 1 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
      if( buf[10+i] != (base+10+i) ) {
        printf("%i: test 1 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+i] != i ) {
        printf("%i: test 1 - buf[%i] = %lu instead of %i\n", commrank, 5+i, buf[5+i], i);
      }
    }
  }

  /* init for second test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* second test: contiguous with other datatype intrinisics */
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 10, MPI_FLOAT, 1 /* rank */, 5 /* offset */, 10 /* count */, MPI_FLOAT, win );
  }

  foMPI_Win_fence( 0, win );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 2 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
      if( buf[10+i] != (base+10+i) ) {
        printf("%i: test 2 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+i] != i ) {
        printf("%i: test 2 - buf[%i] = %lu instead of %i\n", commrank, 5+i, buf[5+i], i);
      }
    }
  }

  /* init for third test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* third test: contiguous with contiguous derived datatype */

  MPI_Type_contiguous( 5, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 5 /* offset */, 1 /* count */, origin_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 3 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
      if( buf[10+i] != (base+10+i) ) {
        printf("%i: test 3 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+i] != i ) {
        printf("%i: test 3 - buf[%i] = %lu instead of %i\n", commrank, 5+i, buf[5+i], i);
      }
    }
  }

  /* init for fourth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* fourth test: target datatype is contiguous, origin datatype is not */
  MPI_Type_vector( 5, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  MPI_Type_contiguous( 5, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 5 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 4 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
      if( buf[10+i] != (base+10+i) ) {
        printf("%i: test 4 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+i] != 2*i ) {
        printf("%i: test 4 - buf[%i] = %lu instead of %i\n", commrank, 5+i, buf[5+i], 2*i);
      }
    }
  }

  /* init for fifth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* fifth test: target datatype is non-contiguous, origin datatype is contiguous */
  MPI_Type_vector( 5, 1, 2, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  MPI_Type_contiguous( 5, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 5 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[5+2*i+1] != (base+5+2*i+1) ) {
        printf("%i: test 5 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i+1, buf[5+2*i+1], base+5+2*i+1);
      }
      if( buf[i] != (base+i) ) {
        printf("%i: test 5 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+2*i] != i ) {
        printf("%i: test 5 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i, buf[5+2*i], i);
      }
    }
  }

  /* init for sixth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* sixth test: same as fifth test, just to test the cache feature */
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 5 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[5+2*i+1] != (base+5+2*i+1) ) {
        printf("%i: test 6 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i+1, buf[5+2*i+1], base+5+2*i+1);
      }
      if( buf[i] != (base+i) ) {
        printf("%i: test 6 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+2*i] != i ) {
        printf("%i: test 6 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i, buf[5+2*i], i);
      }
    }
  }

  /* init for seventh test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* seventh test: both types are non-contiguous, but the same */
  
  MPI_Type_vector( 5, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 5 /* offset */, 1 /* count */, origin_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[5+2*i+1] != (base+5+2*i+1) ) {
        printf("%i: test 7 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i+1, buf[5+2*i+1], base+5+2*i+1);
      }
      if( buf[i] != (base+i) ) {
        printf("%i: test 7 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+2*i] != 2*i ) {
        printf("%i: test 7 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i, buf[5+2*i], 2*i);
      }
    }
  }

  /* init for eighth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* eighth test: same as seventh, just to test the cache feature */
  
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 5 /* offset */, 1 /* count */, origin_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 5 ; i++ ) {
      if( buf[5+2*i+1] != (base+5+2*i+1) ) {
        printf("%i: test 8 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i+1, buf[5+2*i+1], base+5+2*i+1);
      }
      if( buf[i] != (base+i) ) {
        printf("%i: test 8 - buf[%i] = %lu instead of %i\n", commrank, 10+i, buf[10+i], base+10+i);
      }
      if( buf[5+2*i] != 2*i ) {
        printf("%i: test 8 - buf[%i] = %lu instead of %i\n", commrank, 5+2*i, buf[5+2*i], 2*i);
      }
    }
  }

  /* init for nineth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* nineth test: both types are non-contiguous and not the same */
  blk_length[0] = 3;
  blk_length[1] = 3;
  blk_length[2] = 1;

  blk_displ[0] = 0;
  blk_displ[1] = 4;
  blk_displ[2] = 8;

  MPI_Type_indexed( 3, blk_length, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  MPI_Type_vector( 7, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 2 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }
    if( buf[5] != (base+5) ) {
      printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, 5, buf[5], base+5);
    }
    if( buf[9] != (base+9) ) {
      printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, 9, buf[9], base+9);
    }
    for( i=11 ; i < 15 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }

    for( i=0 ; i < 3 ; i++ ) {
      if( buf[2+i] != (2*i) ) {
        printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, 2+i, buf[2+i], 2*i);
      }
    }
    for( i=0 ; i < 3 ; i++ ) {
      if( buf[6+i] != (2*i+6) ) {
        printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, 6+i, buf[6+i], 2*i+6);
      }
    }
    if( buf[10] != (12) ) {
      printf("%i: test 9 - buf[%i] = %lu instead of %i\n", commrank, 10, buf[10], 12);
    }
  }

  /* init for tenth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* tenth test: both types are non-contiguous and not the same, with a fixed size configuration of FIXED_SIZE = 2 the last block should be copy+put */
  blk_length[0] = 3;
  blk_length[1] = 2;
  blk_length[2] = 2;

  blk_displ[0] = 0;
  blk_displ[1] = 4;
  blk_displ[2] = 7;

  MPI_Type_indexed( 3, blk_length, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  MPI_Type_vector( 7, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 2 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }
    if( buf[5] != (base+5) ) {
      printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, 5, buf[5], base+5);
    }
    if( buf[8] != (base+8) ) {
      printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, 8, buf[8], base+8);
    }
    for( i=11 ; i < 15 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }

    for( i=0 ; i < 2 ; i++ ) {
      if( buf[2+i] != (2*i) ) {
        printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, 2+i, buf[2+i], 2*i);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[6+i] != (2*i+6) ) {
        printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, 6+i, buf[6+i], 2*i+6);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[9+i] != (2*i+10) ) {
        printf("%i: test 10 - buf[%i] = %lu instead of %i\n", commrank, 9+i, buf[9+i], 2*i+10);
      }
    }
  }

  /* init for eleventh test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* eleventh test: both types are non-contiguous and not the same, with a fixed size configuration of FIXED_SIZE = 2 the last block should be copy+put
   * same as the last one but with a litte more complex indexed type to check if the implementation does coalescing */
  blk_length[0] = 3;
  blk_length[1] = 2;
  blk_length[2] = 1; /* last block is split in two */
  blk_length[3] = 1;

  blk_displ[0] = 0;
  blk_displ[1] = 4;
  blk_displ[2] = 7;
  blk_displ[3] = 8;

  MPI_Type_indexed( 4, blk_length, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  MPI_Type_vector( 7, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 2 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }
    if( buf[5] != (base+5) ) {
      printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, 5, buf[5], base+5);
    }
    if( buf[8] != (base+8) ) {
      printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, 8, buf[8], base+8);
    }
    for( i=11 ; i < 15 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }

    for( i=0 ; i < 2 ; i++ ) {
      if( buf[2+i] != (2*i) ) {
        printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, 2+i, buf[2+i], 2*i);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[6+i] != (2*i+6) ) {
        printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, 6+i, buf[6+i], 2*i+6);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[9+i] != (2*i+10) ) {
        printf("%i: test 11 - buf[%i] = %lu instead of %i\n", commrank, 9+i, buf[9+i], 2*i+10);
      }
    }
  }

  /* init for twelveth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* twelveth test: both types are non-contiguous and not the same, with a fixed size configuration of FIXED_SIZE = 2 the last block should be copy+put
   * same as the last one but the displacements are not sorted -> force sorting in the implementation */
  blk_length[3] = 3;
  blk_length[1] = 2;
  blk_length[2] = 1;
  blk_length[0] = 1;

  blk_displ[3] = 0;
  blk_displ[1] = 4;
  blk_displ[2] = 7;
  blk_displ[0] = 8;

  MPI_Type_indexed( 4, blk_length, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  blk_displ[0] = 12;
  blk_displ[1] = 6;
  blk_displ[2] = 8;
  blk_displ[3] = 10;
  blk_displ[4] = 0;
  blk_displ[5] = 2;
  blk_displ[6] = 4; 

  MPI_Type_create_indexed_block( 7, 1, blk_displ, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 2 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  if( commrank == 1 ) {
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }
    if( buf[5] != (base+5) ) {
      printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, 5, buf[5], base+5);
    }
    if( buf[8] != (base+8) ) {
      printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, 8, buf[8], base+8);
    }
    for( i=11 ; i < 15 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }

    for( i=0 ; i < 2 ; i++ ) {
      if( buf[2+i] != (2*i) ) {
        printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, 2+i, buf[2+i], 2*i);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[6+i] != (2*i+6) ) {
        printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, 6+i, buf[6+i], 2*i+6);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[9+i] != (2*i+10) ) {
        printf("%i: test 12 - buf[%i] = %lu instead of %i\n", commrank, 9+i, buf[9+i], 2*i+10);
      }
    }
  }
  
   /* init for thirteenth test */
  base = commrank * 15;
  for( i=0 ; i<15 ; i++ ) {
    buf[i] = base + i;
  }

  /* thirteenth test: same as the last one, checking caching */
  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 2 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  if( commrank == 1 ) {
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }
    if( buf[5] != (base+5) ) {
      printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, 5, buf[5], base+5);
    }
    if( buf[8] != (base+8) ) {
      printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, 8, buf[8], base+8);
    }
    for( i=11 ; i < 15 ; i++ ) {
      if( buf[i] != (base+i) ) {
        printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, i, buf[i], base+i);
      }
    }

    for( i=0 ; i < 2 ; i++ ) {
      if( buf[2+i] != (2*i) ) {
        printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, 2+i, buf[2+i], 2*i);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[6+i] != (2*i+6) ) {
        printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, 6+i, buf[6+i], 2*i+6);
      }
    }
    for( i=0 ; i < 2 ; i++ ) {
      if( buf[9+i] != (2*i+10) ) {
        printf("%i: test 13 - buf[%i] = %lu instead of %i\n", commrank, 9+i, buf[9+i], 2*i+10);
      }
    }
  } 
  
  /* fourteenth test: force an overflow of the fixed size buffer */
  blk_length[0] = 3;
  blk_length[1] = 3;
  blk_length[2] = 1;

  blk_displ[0] = 0;
  blk_displ[1] = 4;
  blk_displ[2] = 8;

  MPI_Type_indexed( 3, blk_length, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  MPI_Type_vector( 7, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 0 /* offset */, 1 /* count */, target_datatype, win );
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 2 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  /* fifteenth test: test most general case where the target is contiguous (operations should go through all three stages) */

  MPI_Type_vector( 8, 1, 2, MPI_UINT64_T, &target_datatype );
  MPI_Type_create_resized( target_datatype, 0, sizeof(uint64_t), &origin_datatype );
  foMPI_Type_free( &target_datatype );
  
  blk_displ[0] = 0;
  blk_displ[1] = 1;
  MPI_Type_create_indexed_block( 2, 1, blk_displ, origin_datatype, &target_datatype );
  MPI_Type_commit( &target_datatype );
  foMPI_Type_free( &origin_datatype );

  MPI_Type_vector( 6, 1, 2, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 0 /* offset */, 6 /* count */, MPI_UINT64_T, win );
    foMPI_Put( buf, 1, target_datatype, 1 /* rank */, 0 /* offset */, 16 /* count */, MPI_UINT64_T, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  /* sixteenth test: test most general case with both types non-contiguous */

  MPI_Type_vector( 6, 1, 2, MPI_UINT64_T, &temp_datatype );
  MPI_Type_create_resized( temp_datatype, 0, sizeof(uint64_t), &target_datatype );
  MPI_Type_commit( &temp_datatype );

  blk_displ[0] = 0;
  blk_displ[1] = 1;
  MPI_Type_create_indexed_block( 2, 1, blk_displ, target_datatype, &origin_datatype );
  MPI_Type_commit( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  blk_displ[0] = 0;
  blk_displ[1] = 4;
  blk_displ[2] = 8;
  blk_displ[3] = 12;
  MPI_Type_create_indexed_block( 4, 3, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, temp_datatype, 1 /* rank */, 0 /* offset */, 6 /* count */, MPI_UINT64_T, win );    
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 0 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );
  foMPI_Type_free( &temp_datatype ); 
 
  /* seventeenth test: last two tests, datatype contains a block which is bigger than the fixed size buffer */
  /* target contiguous */
  blk_displ[0] = 0;
  blk_displ[1] = 11;

  blk_length[0] = 10;
  blk_length[1] = 1;

  MPI_Type_indexed( 2, blk_length, blk_displ, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 0 /* offset */, 12 /* count */, MPI_UINT64_T, win );
  }

  foMPI_Win_fence( 0, win );
  
  foMPI_Type_free( &origin_datatype );

  /* seventeenth test: last two tests, datatype contains a block which is bigger than the fixed size buffer */
  /* both types non-contiguous */

  blk_displ[0] = 0;
  blk_displ[1] = 11;
  blk_displ[2] = 13;

  blk_length[0] = 10;
  blk_length[1] = 1;
  blk_length[2] = 1;

  MPI_Type_indexed( 3, blk_length, blk_displ, MPI_UINT64_T, &origin_datatype );
  MPI_Type_commit( &origin_datatype );

  blk_displ[0] = 0;
  blk_displ[1] = 11;

  blk_length[0] = 10;
  blk_length[1] = 2;

  MPI_Type_indexed( 2, blk_length, blk_displ, MPI_UINT64_T, &target_datatype );
  MPI_Type_commit( &target_datatype );

  foMPI_Win_fence( 0, win );

  if( commrank == 0) {
    foMPI_Put( buf, 1, origin_datatype, 1 /* rank */, 0 /* offset */, 1 /* count */, target_datatype, win );
  }

  foMPI_Win_fence( 0, win );

  foMPI_Type_free( &origin_datatype );
  foMPI_Type_free( &target_datatype );

  foMPI_Win_free( &win );

  foMPI_Finalize();

  return 0;
}
