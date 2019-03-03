      subroutine test_mpi3rma_acc_sum_pscw( base, array_size, DIM1, win, comm )

      use utilities, only: check_fompi_status

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

      integer :: i
      integer :: rank
      integer :: commsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: start

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      integer, dimension(3) :: ranges
      integer :: group
      integer :: newgroup

      character(50) :: filename

      write (filename,*) "acc_sum_pscw.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )
  
      if ( rank .eq. 0 ) then
        ranges(1) = 1
        ranges(2) = commsize-1
        ranges(3) = 1
      else
        ranges(1) = 0
        ranges(2) = 0
        ranges(3) = 1
      endif

      call MPI_Comm_group(comm, group, mpierr)
      call MPI_Group_range_incl(group, 1, ranges, newgroup, mpierr)
      call MPI_Group_free( group, mpierr )

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if ( rank .eq. 0 ) then
          call foMPI_Get_address( base(1), start, mpierr)
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 0
      endif

      do i=1,DIM1
        base(i) = rank+i
      enddo

      if( rank .eq. 0 ) then
        call foMPI_Win_post(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 67, rank)
    
        call foMPI_Win_wait(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 70, rank)
      else
        call foMPI_Win_start(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 73, rank)
    
        call foMPI_Accumulate( base(1), DIM1, MPI_INTEGER, 0, start, DIM1, MPI_INTEGER, foMPI_SUM, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 76, rank)
    
        call foMPI_Win_complete(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 79, rank)
      endif

      call MPI_Group_free( newgroup, mpierr )

      if ( rank .eq. 0 ) then
        do i=1,DIM1
          if( base(i) .ne. (commsize-1)*((commsize-1)+1)/2+i*commsize ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 86, ": element ", i, " is ", base(i), &
              " instead of ", (commsize-1)*((commsize-1)+1)/2+i*commsize, "."
            STOP
          endif
        enddo
      endif

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_acc_sum_pscw 
