// Copyright (c) 2014 ETH-Zurich. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "fompi.h"

int foMPI_Win_set_name(foMPI_Win win, const char *win_name) {

  size_t size;

  if (win->name != NULL ) {
	  _foMPI_FREE( win->name );
  }

  size = strlen( win_name );

  if( size > MPI_MAX_OBJECT_NAME ) {
    win->name = _foMPI_ALLOC( (MPI_MAX_OBJECT_NAME+1) * sizeof(char) );
    strncpy( &(win->name[0]), win_name, MPI_MAX_OBJECT_NAME );
    win->name[MPI_MAX_OBJECT_NAME] = '\0';
    return foMPI_NAME_ABBREVIATED;
  } else {
    win->name = _foMPI_ALLOC( (size+1) * sizeof(char) );
    strcpy( &(win->name[0]), win_name );
  }

  return MPI_SUCCESS;
}

int foMPI_Win_get_name(foMPI_Win win, char *win_name, int *resultlen) {

  if( win->name == NULL ) {
    win_name[0] = '\0';
    *resultlen = 0;
  } else {
    *resultlen = strlen( win->name );
    strcpy( win_name, win->name );
  }

  return MPI_SUCCESS;
}
