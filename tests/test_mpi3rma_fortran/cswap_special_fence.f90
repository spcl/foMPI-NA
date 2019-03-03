      subroutine test_mpi3rma_cswap_special_fence( base, array_size, win, comm )

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
      integer(kind=8), dimension(array_size), intent(inout) :: base
      integer, intent(in) :: win
      integer, intent(in) :: comm

      integer :: i
      integer :: rank
      integer :: commsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: start

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr
      integer(kind=8), dimension(:), allocatable :: cswap_result
      integer(kind=8) :: compare
      integer(kind=8) :: cswap_buf

      character(50) :: filename

      write (filename,*) "cswap_special_fence.f90"

      call MPI_Comm_rank(comm, rank, mpierr )
      call MPI_Comm_size(comm, commsize, mpierr )

      if ( rank .eq. 0 ) then
        allocate( cswap_result(commsize), stat=mpierr )
      endif

      base(1) = mod(rank,2)
      compare = 0
      cswap_buf = 1

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 40, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif

      if ( create_flavor == foMPI_WIN_FLAVOR_DYNAMIC ) then
        call foMPI_Get_address( base(1), start, mpierr )
        if (rank .eq. 0) then
          allocate( base_adr(commsize), stat=mpierr )
        endif
        call MPI_Gather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 0
      endif

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      if (rank .eq. 0) then
        do i = 0,commsize-1
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Compare_and_swap( cswap_buf, compare, cswap_result(i+1), MPI_INTEGER8, i, base_adr(i+1), win, mpierr )
          else
            call foMPI_Compare_and_swap( cswap_buf, compare, cswap_result(i+1), MPI_INTEGER8, i, start, win, mpierr )
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 66, rank)
        enddo
        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          deallocate( base_adr, stat=mpierr )
        endif
      endif

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)
  
      if (rank == 0 ) then
        do i = 0,commsize-1
          if ( cswap_result(i+1) .ne. mod(i,2) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 61, ": MPI_Compare_swap error - result ", &
              cswap_result(i+1), " doesn't match ", mod(i,2), "."
            STOP
          endif
        enddo
        deallocate( cswap_result, stat=mpierr )
      endif

      if ( base(1) .ne. 1 ) then
        write (*,*) "Error on rank ", rank, " in ", trim(filename), &
          " line ", 61, ": element ", 1, " is ", base(1), &
          " instead of ", 1, "."
        STOP
      endif

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_cswap_special_fence
