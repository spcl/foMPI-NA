      subroutine test_mpi3rma_put_fence( base, array_size, DIM1, win, comm )

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

      integer :: commsize
      integer :: rank
      integer :: mpierr
      integer :: i
      integer(kind=MPI_ADDRESS_KIND) :: start

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      character(50) :: filename

      write (filename,*) "put_fence.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )
  
      do i=1,array_size
        base(i) = commsize+i
      enddo

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 36, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        call foMPI_Get_address( base(DIM1+1), start, mpierr )
        call MPI_Sendrecv_replace( start, 1, MPI_AINT, commsize - rank - 1, 0, &
          commsize - rank - 1, 0, comm, MPI_STATUS_IGNORE, mpierr )
      else
        start = DIM1
      endif

      call foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)
      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 52, rank)

      call foMPI_Put( base(1), DIM1, MPI_INTEGER, commsize - rank - 1, start, DIM1, MPI_INTEGER, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 55, rank)

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 58, rank)

      do i=1,DIM1
        if(base(i) .ne. commsize+i) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 61, ": element ", i, " is ", base(i), &
            " instead of ", commsize+i, "."
          STOP
        endif
      enddo
      do i=1,DIM1
        if(base(DIM1+i) .ne. commsize+i) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 69, ": element ", i, " is ", base(DIM1+i), &
            " instead of ", commsize+i, "."
          STOP
        endif
      enddo
      do i=2*DIM1+1,3*DIM1
        if(base(i) .ne. commsize+i) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 78, ": element ", i, " is ", base(i), &
            " instead of ", commsize+i, "."
          STOP
        endif
      enddo

      call MPI_Barrier( comm, mpierr )
  
      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

     end subroutine test_mpi3rma_put_fence 
