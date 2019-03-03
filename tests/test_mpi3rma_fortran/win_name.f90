! This test is derived from the win_name test of the RMA tests of mpich-1.5b2.

      subroutine test_mpi3rma_win_name( win )

      use utilities, only: check_fompi_status

#if INCLUDE_TYPE==1
      implicit none

      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi

      implicit none
#endif

      integer, intent(in) :: win

      character(MPI_MAX_OBJECT_NAME) :: window_name
      character(MPI_MAX_OBJECT_NAME) :: get_window_name
      integer :: length
      integer :: mpierr
      integer :: cnt = 0

      character(50) :: filename

      write (filename,*) "win_name.f90"

      write (window_name,*) "win-", cnt
      cnt = cnt + 1

      call foMPI_Win_set_name( win, window_name, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 27, 0 ) ! only rank 0 calls this test routine

      call foMPI_Win_get_name( win, get_window_name, length, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 30, 0 ) ! only rank 0 calls this test routine

      if( window_name .eq. get_window_name ) then
        write (*,*) "No Error"
      else
        write (*,*) "Error: Got name ", trim(get_window_name), " instead of ", &
          trim(window_name), "."
      endif

      end subroutine test_mpi3rma_win_name
