      subroutine test_mpi3rma_put_pscw( base, array_size, DIM1, win, comm )

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

      write (filename,*) "put_pscw.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 1) then
          call foMPI_Get_address( base(3*DIM1), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 1, comm, mpierr )
      else
        start = 3*DIM1-1
      endif

      ranges(1) = mod(rank+1,2)
      ranges(2) = commsize-1
      ranges(3) = 2
      call MPI_Comm_group( comm, group, mpierr )
      call MPI_Group_range_incl( group, 1, ranges, newgroup, mpierr )
      call MPI_Group_free( group, mpierr )

      if ( mod(rank,2) .eq. 0 ) then
        call foMPI_Win_start(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 59, rank)

        base(1) = 4*DIM1
      
        call foMPI_Put( base(1), 1, MPI_INTEGER, 1, start, 1, MPI_INTEGER, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 64, rank)

        call foMPI_Win_complete(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 67, rank)
      else
        call foMPI_Win_post(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 70, rank)

        call foMPI_Win_wait(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 73, rank)
      endif

      call MPI_Group_free( newgroup, mpierr )

      if ( rank .eq. 1 ) then
        if (base(3*DIM1) .ne. 4*DIM1) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 79, ": element ", 3*DIM1, " is ", base(3*DIM1), &
            " instead of ", 4*DIM1, "."
          STOP
        endif
      endif

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_put_pscw
