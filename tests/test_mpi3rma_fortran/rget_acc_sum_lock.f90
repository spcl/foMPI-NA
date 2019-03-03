      subroutine test_mpi3rma_rget_acc_sum_lock( base, array_size, DIM1, win, comm )

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

      integer, dimension(:), allocatable :: request
      integer, dimension(:), allocatable :: recv_buf
      integer :: j
      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr

      character(50) :: filename

      write (filename,*) "rget_acc_sum_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 37, rank)

      do i=1,DIM1
        base(DIM1+i) = i
        if (rank .eq. 0) then
          base(i) = i
        endif
      enddo

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
        call foMPI_Get_address( base(DIM1+1), start, mpierr )
        call MPI_Gather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, 0, comm, mpierr )
      else
        start = DIM1
      endif

      call MPI_Barrier( comm, mpierr ) ! end of the init phase
  
      if (rank .eq. 0) then
        allocate( request(commsize), stat=mpierr )
        allocate( recv_buf(DIM1*commsize), stat=mpierr )

        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, 0, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)
        enddo

        do i=0,commsize-1
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Rget_accumulate( base(1), DIM1, MPI_INTEGER, &
              recv_buf(i*DIM1+1), DIM1, MPI_INTEGER, i, base_adr(i+1), &
              DIM1, MPI_INTEGER, foMPI_SUM, win, request(i+1), mpierr )
          else
            call foMPI_Rget_accumulate( base(1), DIM1, MPI_INTEGER, &
              recv_buf(i*DIM1+1), DIM1, MPI_INTEGER, i, start, DIM1, &
              MPI_INTEGER, foMPI_SUM, win, request(i+1), mpierr )
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 83, rank)
        enddo
    
        do i=commsize-1,0,-1
          call foMPI_Test( request(i+1), flag, MPI_STATUS_IGNORE, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 88, rank)
          if (flag .eqv. .true.) then
!            write (*,*) rank, ": finished ", i
          endif
        enddo
  
        do i=0,commsize-1
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 96, rank)
        enddo
  
        do i=0,commsize-1
          if ( request(i+1) .ne. foMPI_REQUEST_NULL ) then
            call foMPI_Wait( request(i+1), MPI_STATUS_IGNORE, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 102, rank)
          endif
        enddo
        deallocate( request, stat=mpierr )
      endif

      call MPI_Barrier( comm, mpierr ) ! wait for the end of accesses

      if ( rank .eq. 0 ) then
        do j=0,commsize-1
          do i=1,DIM1
            if( recv_buf(j*DIM1+i) .ne. i ) then
              write (*,*) "Error on rank ", rank, " in ", trim(filename), &
                " line ", 113, ": element ", j*DIM1+i, " is ", recv_buf(j*DIM1+i), &
                " instead of ", i, "."
              STOP
            endif
          enddo
        enddo
        deallocate( recv_buf, stat=mpierr )
        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          deallocate( base_adr, stat=mpierr )
        endif
      endif

      do i=1,DIM1
        if( base(DIM1+i) .ne. (2*i) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 128, ": element ", DIM1+i, " is ", base(DIM1+i), &
            " instead of ", 2*i, "."
          STOP
        endif
      enddo

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_rget_acc_sum_lock
