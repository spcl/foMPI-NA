      subroutine test_mpi3rma_pscw1( win, comm )

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
      integer, intent(in) :: comm
  
      integer :: rank
      integer :: commsize
      integer :: mpierr

      integer, dimension(3) :: ranges
      integer :: group
      integer :: newgroup

      character(50) :: filename

      write (filename,*) "pscw1.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      ranges(1) = mod(rank+1,2)
      ranges(2) = commsize-1
      ranges(3) = 2
      call MPI_Comm_group(comm, group, mpierr)
      call MPI_Group_range_incl(group, 1, ranges, newgroup, mpierr)
      call MPI_Group_free( group, mpierr )

      call foMPI_Win_start(newgroup, 0, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35, rank)
  
      call foMPI_Win_post(newgroup, 0, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 38, rank)

      call foMPI_Win_complete(win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 41, rank)

      call foMPI_Win_wait(win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 44, rank)

      call MPI_Group_free( newgroup, mpierr )

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write(*,*) "No Error"
      endif

      end subroutine test_mpi3rma_pscw1
