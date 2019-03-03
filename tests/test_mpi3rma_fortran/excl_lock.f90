! this isn't one of the general tests, instead it is used to test some implementation details (list of exclusive locks),
! including some ugly access of the window struct by the test itself 
! so if this test breaks, if you change the implementation of exclusive lock bookkeeping, no worries

      subroutine test_mpi3rma_excl_lock( comm )

      use utilities, only: check_fompi_status
      
#if INCLUDE_TYPE==1
      implicit none
      
      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi
      
      implicit none
#endif

      integer, intent(in) :: comm

      integer :: rank, commsize
      integer :: test = 0
      integer :: put_data = 1
      integer :: win
      integer :: i

      integer(kind=MPI_ADDRESS_KIND) :: win_size
      integer(kind=MPI_ADDRESS_KIND) :: target_disp ! fortran uses for
      !constant parameter integer(kind=4), which is not according to the
      !MPI-3.0 standard

      integer :: mpierr

      character(50) :: filename

      test = 0
      put_data = 1

      target_disp = 0

      write (filename,*) "excl_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      ! the part of the foMPI_Win_free implementation we want to test is the same of all three window types
      ! assuming size of an integer is 4
      win_size = 4
      call foMPI_Win_create( test, win_size, 4, MPI_INFO_NULL, comm, win, mpierr )

      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 44, rank)

      if (rank .eq. 0 ) then

        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)

          ! so that we actually do something, in case we implement a lazy
          ! approach later
          call foMPI_Put( put_data, 1, MPI_INTEGER, i, target_disp, 1, MPI_INTEGER, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 54, rank)
        enddo

        ! first test: see if we can remove those exclusive locks from the local list without problems, if we remove them inorder
        do i=0,commsize-1
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 61, rank);
        enddo

      endif

      call MPI_Barrier( comm, mpierr )

      if( test .ne. put_data ) then
        write (*,*) "Error on rank ", rank, &
          " in excl_lock.f90 at the first test: element is ", test, &
          " instead of ", put_data, "."
        STOP
      endif
      test = 0

!  don't know howto to access structs from fortran
!  if ( win->excl_locks != NULL ) { /* not really a clean solution */
!    fprintf(stderr, "Error on rank %i in %s line %i: list of exclusive locks isn't empty.\n", rank, __FILE__, __LINE__ );
!    exit(EXIT_FAILURE);
!  }

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0 ) then

        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 88, rank)

          ! so that we actually do something, in case we implement a lazy
          ! approach later
          call foMPI_Put( put_data, 1, MPI_INTEGER, i, target_disp, 1, MPI_INTEGER, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 93, rank)
        enddo

        ! second test: see if we can remove those exclusive locks from the local list without problems, if we remove them in reverse order
        do i=commsize-1,0,-1
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 91, rank)
        enddo

      endif

      call MPI_Barrier( comm, mpierr )

      if( test .ne. put_data ) then
        write (*,*) "Error on rank ", rank, &
          " in excl_lock.f90 at the second test: element is ", test, &
          " instead of ", put_data, "."
        STOP
      endif
      test = 0

!  if ( win->excl_locks != NULL ) { /* not really a clean solution */
!    fprintf(stderr, "Error on rank %i in %s line %i: list of exclusive locks isn't empty.\n", rank, __FILE__, __LINE__ );
!    exit(EXIT_FAILURE);
!  }

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0 ) then

        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 125, rank)

          ! so that we actually do something, in case we implement a lazy
          ! approach later
          call foMPI_Put( put_data, 1, MPI_INTEGER, i, target_disp, 1, MPI_INTEGER, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 130, rank)
        enddo

        ! third test: see if we can remove those exclusive locks from the local list without problems, if we first remove the even ranks and then the odd ones
        do i=0,commsize-1,2
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 135, rank)
        enddo
        do i=1,commsize-1,2
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 140, rank)
        enddo
      endif

      call MPI_Barrier( comm, mpierr )

      if( test .ne. put_data ) then
        write (*,*) "Error on rank ", rank, &
          " in excl_lock.f90 at the third test: element is ", test, &
          " instead of ", put_data, "."
        STOP
      endif
      test = 0

!  if ( win->excl_locks != NULL ) { /* not really a clean solution */
!    fprintf(stderr, "Error on rank %i in %s line %i: list of exclusive locks isn't empty.\n", rank, __FILE__, __LINE__ );
!    exit(EXIT_FAILURE);
!  }

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0 ) then

        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, MPI_INFO_NULL, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 165, rank)

          ! so that we actually do something, in case we implement a lazy
          ! approach later
          call foMPI_Put( put_data, 1, MPI_INTEGER, i, target_disp, 1, MPI_INTEGER, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 170, rank)
        enddo

        ! fourth test: no unlock of the exclusive locks, so this has to be done by foMPI_Win_free (no actual consistency),
        ! just make sure that the implementation doesn't break at this point (and hopefully doesn't leak memory)
      endif

      call foMPI_Win_free( win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 178, rank)

      ! fifth test: just one exclusive lock isn't unlocked

      ! assuming size of an integer is 4
      win_size = 4
      call foMPI_Win_create( test, win_size, 4, MPI_INFO_NULL, comm, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 184, rank)

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, MPI_INFO_NULL, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 187, rank)

      ! so that we actually do something, in case we implement a lazy approach
      ! later
      call foMPI_Put( put_data, 1, MPI_INTEGER, rank, target_disp, 1, MPI_INTEGER, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 192, rank)

      call foMPI_Win_free( win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 195, rank)

      ! sixth test: without a exclusive lock

      ! assuming size of an integer is 4
      win_size = 4
      call foMPI_Win_create( test, win_size, 4, MPI_INFO_NULL, comm, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 201, rank)

      call foMPI_Win_free( win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 204, rank)

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif 

      end subroutine test_mpi3rma_excl_lock
