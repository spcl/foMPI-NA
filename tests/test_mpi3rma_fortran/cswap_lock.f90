      subroutine test_mpi3rma_cswap_lock( base, array_size, win, comm )

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
      integer, intent(in) :: win
      integer, intent(in) :: comm
  
      integer :: i
      integer :: rank
      integer :: commsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: start

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      integer, dimension(:), allocatable :: result_buf
      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr;
      integer :: comparand

      character(50) :: filename

      write (filename,*) "cswap_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35, rank)

      base(1) = mod(rank,2)

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 40, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 43, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if ( rank .eq. 0 ) then
          allocate( base_adr(commsize), stat=mpierr )
        endif
        call foMPI_Get_address( base(1), start, mpierr )
        call MPI_Gather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 0
      endif

      call MPI_Barrier( comm, mpierr ) ! this ends the init phase

      if (rank .eq. 0) then
        comparand = 0
        base(2) = 1

        allocate( result_buf(commsize), stat=mpierr )

        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, i, 0, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 69, rank)
        enddo
  
        do i=0,commsize-1
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Compare_and_swap( base(2), comparand, result_buf(i+1), MPI_INTEGER, i, base_adr(i+1), win, mpierr )
          else
            call foMPI_Compare_and_swap( base(2), comparand, result_buf(i+1), MPI_INTEGER, i, start, win, mpierr )
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 78, rank)
        enddo
    
        do i=0,commsize-1
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 83, rank)
        enddo
    
        do i=0,commsize-1
          if ( result_buf(i+1) .ne. mod(i,2) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 87, ": element ", i+1, " is ", result_buf(i+1), &
              " instead of ", mod(i,2), "."
            STOP
          endif
        enddo

        deallocate( result_buf, stat=mpierr )
        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          deallocate( base_adr, stat=mpierr )
        endif
      endif
 
      call MPI_Barrier( comm, mpierr ) ! wait for the end of the accesses

      if ( base(1) .ne. 1 ) then
        write (*,*) "Error on rank ", rank, " in ", trim(filename), &
          " line ", 103, ": element ", 1, " is ", base(1), &
          " instead of ", 1, "."
        STOP
      endif

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_cswap_lock
