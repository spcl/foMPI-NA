      subroutine test_mpi3rma_pscw2( win, comm )

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

      logical :: flag
      integer, dimension(3) :: ranges
      integer :: group
      integer :: newgroup

      character(50) :: filename

      write (filename,*) "pscw2.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      ranges(1) = mod(rank+1,2)
      ranges(2) = commsize-1
      ranges(3) = 2
      call MPI_Comm_group(comm, group, mpierr)
      call MPI_Group_range_incl(group, 1, ranges, newgroup, mpierr)
      call MPI_Group_free( group, mpierr )

      if ( mod(rank,2) .eq. 0 ) then
        call foMPI_Win_start(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 37, rank)
    
        call foMPI_Win_complete(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 40, rank)
    
        call foMPI_Win_post(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 43, rank)
  
        call foMPI_Win_wait(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)
      else
        call foMPI_Win_post(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 49, rank)
  
        call foMPI_Win_start(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 52, rank)

        call foMPI_Win_complete(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 55, rank)
    
        do 
          call foMPI_Win_test(win, flag, mpierr)
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 59, rank)
          if (flag .eqv. .true.) then
            exit
          endif
        enddo
      endif

      call MPI_Group_free( newgroup, mpierr )

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_pscw2
