      subroutine test_mpi3rma_racc_sum_lock( base, array_size, DIM1, win, comm )

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

      integer, dimension(:), allocatable :: request;
      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr

      character(50) :: filename

      write (filename,*) "racc_sum_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35, rank)

      do i=1,DIM1
        base(i) = rank+i
        base(DIM1+i) = 0
      enddo

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 43, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        allocate( base_adr(commsize), stat=mpierr )
        call foMPI_Get_address( base(DIM1+1), start, mpierr )
        call MPI_Allgather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, comm, mpierr ) ! finishes the init phase
      else
        start = DIM1
        call MPI_Barrier( comm, mpierr ) ! finishes the init phase
      endif

      allocate( request(commsize), stat=mpierr )

      do i=0,commsize-1
        call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 65, rank)
      enddo
  
      do i=0,commsize-1
        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          call foMPI_Raccumulate( base(1), DIM1, MPI_INTEGER, i, &
            base_adr(i+1), DIM1, MPI_INTEGER, foMPI_SUM, win, &
            request(i+1), mpierr )
        else
          call foMPI_Raccumulate( base(1), DIM1, MPI_INTEGER, i, start, DIM1, MPI_INTEGER, foMPI_SUM, win, request(i+1), mpierr )
        endif
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)
      enddo

      do i=commsize-1,0,-1
        call foMPI_Test( request(i+1), flag, MPI_STATUS_IGNORE, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 79, rank)
        if (flag .eqv. .true.) then
!          write (*,*) rank, ": finished ", i
        endif
      enddo

      do i=0,commsize-1
        call foMPI_Win_unlock( i, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 87, rank)
      enddo

      do i=0,commsize-1
        if ( request(i+1) .ne. foMPI_REQUEST_NULL ) then
          call foMPI_Wait( request(i+1), MPI_STATUS_IGNORE, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 93, rank)
        endif
      enddo

      call MPI_Barrier( comm, mpierr ) ! wait for end of all accesses

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        deallocate( base_adr )
      endif

      deallocate( request )

      do i=1,DIM1
        if( base(DIM1+i) .ne. (commsize-1)*((commsize-1)+1)/2+i*commsize ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 108, ": element ", DIM1+i, " is ", base(DIM1+i), &
            " instead of ", (commsize-1)*((commsize-1)+1)/2+i*commsize, "."
          STOP
        endif
      enddo

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_racc_sum_lock
