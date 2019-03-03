      subroutine test_mpi3rma_rput_rget_lock( base, array_size, DIM1, win, comm )

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

      integer :: status_rma(MPI_STATUS_SIZE)
      integer, dimension(:), allocatable :: request

      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr

      write (filename,*) "rput_rget_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 37, rank)

      if (rank .eq. 0) then
        base(1:DIM1) = commsize
      else
        base(1:array_size) = 0
      endif

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 49, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
 
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0) then
          allocate( base_adr(commsize), stat=mpierr )
        endif
        call foMPI_Get_address( base(1), start, mpierr )
        call MPI_Gather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, 0, comm, mpierr )
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr ) ! also the signal that the initialization is finished
      else
        start = 0
        call MPI_Barrier( comm, mpierr ) ! guarantees that the initialization is finished
      endif

      if (rank .eq. 0) then
    
        allocate( request(commsize), stat=mpierr )
   
        do i=1,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, 0, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 73, rank)
        enddo

        do i=1,commsize-1
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Rput( base(1), DIM1, MPI_INTEGER, i, base_adr(i+1), DIM1, MPI_INTEGER, win, request(i+1), mpierr )
          else
            call foMPI_Rput( base(1), DIM1, MPI_INTEGER, i, start, DIM1, MPI_INTEGER, win, request(i+1), mpierr )
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 82, rank)
        enddo
    
        do i=commsize-1,1,-1
          call foMPI_Test( request(i+1), flag, status_rma, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 87, rank)
!          if (flag .eqv. .true.) then ! if something is not finished during the epoch
!             write (*,*) rank, ": after rput request ", i, "finished"
!          endif
        enddo
    
        do i=1,commsize-1
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 95, rank)
        enddo
    
        do i=1,commsize-1
          if ( request(i+1) .ne. foMPI_REQUEST_NULL ) then
            call foMPI_Test( request(i+1), flag, status_rma, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 101, rank)
            if (flag .neqv. .true.) then
              write (*,*) rank, ": request ", i, &
                " was not finished after unlock"
              STOP
            endif
          endif
        enddo
    
        deallocate( request, stat=mpierr )
        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          deallocate( base_adr, stat=mpierr )
        endif
      else
        allocate( request(2), stat=mpierr )
    
        call foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 118, rank)
    
        call foMPI_Rget( base(DIM1+1), 1, MPI_INTEGER, 0, start, 1, MPI_INTEGER, win, request(1), mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 121, rank)

        call foMPI_Rget( base(DIM1+2), 1, MPI_INTEGER, 0, start, 1, MPI_INTEGER, win, request(2), mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 124, rank)
  
        call foMPI_Wait( request(2), status_rma, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 127, rank)
  
        call foMPI_Win_unlock( 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 130, rank)
  
        call foMPI_Wait( request(1), MPI_STATUS_IGNORE, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 133, rank)
  
        deallocate( request, stat=mpierr )
      endif

      call MPI_Barrier( comm, mpierr ) ! wait for the end of the accesses

      if ( rank .ne. 0 ) then
        do i=1,DIM1+2
          if( base(i) .ne. commsize ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 142, ": element ", i, " is ", base(i), &
              " instead of ", commsize, "."
            STOP
          endif
        enddo
      endif

      if ( rank == 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_rput_rget_lock
