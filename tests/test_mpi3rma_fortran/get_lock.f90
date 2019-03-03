      subroutine test_mpi3rma_get_lock( base, array_size, DIM1, win, comm )

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

      character(50) :: filename

      write (filename,*) "get_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 32, rank)

      do i=1,DIM1
        base(i) = commsize+i;
        base(DIM1+i) = commsize+i;
        base(2*DIM1+i) = commsize+i;
      enddo

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 41, rank)
      
      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 44, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0 ) then
          call foMPI_Get_address( base(3*DIM1), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr ) ! also the signal that the initialization is finished
      else
        start = 3*DIM1-1
        call MPI_Barrier( comm, mpierr ) ! also the signal that the initialization is finished
      endif

      call foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 60, rank)

      call foMPI_Get( base(1), 1, MPI_INTEGER, 0, start, 1, MPI_INTEGER, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 64, rank)

      call foMPI_Win_unlock( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 67, rank)

      call MPI_Barrier( comm, mpierr ) ! finished all accesses to this epoch

      do i=2,3*DIM1
        if ( base(i) .ne. commsize+mod(i-1,DIM1)+1 ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 72, ": element ", i, " is ", base(i), &
            " instead of ", commsize+mod(i-1,DIM1)+1, "."
          STOP
        endif
      enddo
      if ( base(1) .ne. commsize+DIM1 ) then
        write (*,*) "Error on rank ", rank, " in ", trim(filename), &
          " line ", 79, ": element ", 1, " is ", base(1), &
          " instead of ", commsize+DIM1, "."
        STOP
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_get_lock
