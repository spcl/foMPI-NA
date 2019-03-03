      subroutine test_mpi3rma_rget_lockall( base, array_size, DIM1, win, comm )

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

      integer :: status_rma(MPI_STATUS_SIZE)
      integer, dimension(:), allocatable :: request
  
      integer :: minimum
      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr
 
      character(50) :: filename

      write (filename,*) "rget_lockall.f90"
  
      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 38, rank)

      if (rank .eq. 0 ) then
        base(1:array_size) = 0
      else
        base(1) = rank
      endif

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 47, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0) then
          allocate( base_adr(commsize), stat=mpierr )
        endif
        call foMPI_Get_address( base(1), start, mpierr )
        call MPI_Gather( start, 1, MPI_AINT, base_adr, 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 0
      endif
      call MPI_Barrier( comm, mpierr ) ! guarantees that the initialization is finished

      if (rank .eq. 0) then
        allocate( request(commsize), stat=mpierr )
 
        if (commsize-1 .gt. array_size) then
          minimum = array_size
        else
          minimum = commsize-1
        endif
    
        call foMPI_Win_lock_all(0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 77, rank)

        do i=1,minimum
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Rget( base(i), 1, MPI_INTEGER, i, base_adr(i+1), 1, MPI_INTEGER, win, request(i+1), mpierr )
          else
            call foMPI_Rget( base(i), 1, MPI_INTEGER, i, start, 1, MPI_INTEGER, win, request(i+1), mpierr)
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 85, rank)
        enddo
    
        call foMPI_Win_unlock_all( win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 89, rank)

        do i=1,minimum
          call foMPI_Test( request(i+1), flag, status_rma, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 93, rank)
          if (flag .neqv. .true.) then
            write (*,*) rank, ": request ", i, &
              " was not finished after unlock"
            STOP
          endif
        enddo
  
        deallocate( request, stat=mpierr )

        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          deallocate( base_adr, stat=mpierr )
        endif

        do i=1,minimum
          if( base(i) .ne. i ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 109, ": element ", i, " is ", base(i), &
              " instead of ", i, "."
            STOP
          endif
        enddo
      endif

      call MPI_Barrier( comm, mpierr ) ! wait for the end of the accesses

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_rget_lockall
