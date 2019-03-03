      subroutine test_mpi3rma_get_fence( base, array_size, DIM1, win, comm )

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

      integer :: mpierr
      integer :: i
      integer :: rank
      integer :: commsize
      integer(kind=MPI_ADDRESS_KIND) :: start

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      character(50) :: filename

      write (filename,*) "get_fence.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      do i=1,DIM1
        base(i) = commsize+i
        base(DIM1+i) = commsize+i
        base(2*DIM1+i) = commsize+2*DIM1+i
      enddo

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 38, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval." 
        STOP
      endif

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0 ) then
          call foMPI_Get_address( base(DIM1+1), start, mpierr)
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )
      else
        start = DIM1
      endif

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 54, rank)

      call foMPI_Get( base(2*DIM1+1), DIM1, MPI_INTEGER, 0, start, DIM1, MPI_INTEGER, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      call foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 60, rank)

      do i=1,3*DIM1
        if( base(i) .ne. commsize+mod(i-1,DIM1)+1) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 63, ": element ", i, " is ", base(i), &
            " instead of ", commsize+mod(i-1,DIM1)+1, "."
          STOP
        endif
      enddo

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_get_fence
