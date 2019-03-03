      subroutine test_mpi3rma_lock_exclusive_concurrency( base, array_size, DIM1, win, comm )

      use utilities, only: check_fompi_status, millisleep

#if INCLUDE_TYPE==1
      implicit none
      
      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi
      
      implicit none
#endif

      integer, intent(in) :: array_size
      integer, dimension(array_size), intent(inout) :: base
      integer, intent(in) :: DIM1
      integer, intent(in) :: win
      integer, intent(in) :: comm

      integer :: rank
      integer :: commsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: start
      integer :: test

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      character(50) :: filename

      write (filename,*) "lock_exclusive_concurrency.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      if ( (commsize .lt. 2) .and. (rank .eq. 0) ) then
        write (*,*) "This test needs at least 2 processes."
        STOP
      endif

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 36, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 1) then
          call foMPI_Get_address( base(1), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 1, comm, mpierr )
      else
        start = 0
      endif

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 52, rank)

      base(1) = 0;

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 64, rank)

        do
          call foMPI_Win_lock( foMPI_LOCK_SHARED, 1, 0, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 68, rank)

          call foMPI_Get( test, 1, MPI_INTEGER, 1 , start, 1 , MPI_INTEGER, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 71, rank)

          call foMPI_Win_unlock( 1, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)
!          write (*,*) rank, ": got ", test
          if (test .eq. 1) then
            exit
          endif
        enddo

        call foMPI_Win_unlock( rank, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 82, rank)
      endif

      if ( rank .eq. 1 ) then

        call millisleep(100)

        call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 90, rank)

        base(1) = 1

        call foMPI_Win_unlock( rank, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 95, rank)
      endif

      call MPI_Barrier( comm, mpierr ) ! wait for all accesses to finish

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_lock_exclusive_concurrency
