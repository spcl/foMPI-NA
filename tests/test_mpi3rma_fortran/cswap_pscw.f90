      subroutine test_mpi3rma_cswap_pscw( base, array_size, win, comm )

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

      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr
      integer comparand
      integer, dimension(3) :: ranges
      integer :: group
      integer :: newgroup;
      integer, dimension(:), allocatable :: result_buf

      character(50) :: filename

      write (filename,*) "cswap_pscw.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      base(1) = mod(rank,2)

      if ( rank .eq. 0 ) then
        ranges(1) = 1
        ranges(2) = commsize-1
        ranges(3) = 1
      else 
        ranges(1) = 0
        ranges(2) = 0
        ranges(3) = 1
      endif

      call MPI_Comm_group(comm, group, mpierr)
      call MPI_Group_range_incl(group, 1, ranges, newgroup, mpierr)
      call MPI_Group_free( group, mpierr )

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 54, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0) then
          allocate( base_adr(commsize), stat=mpierr )
        endif
        call foMPI_Get_address( base(1), start, mpierr)
        call MPI_Gather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 0
      endif

      if (rank .eq. 0) then
        allocate( result_buf(commsize), stat=mpierr)

        call foMPI_Win_start(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)
    
        comparand = 0
        base(2) = 1
        do i=1,commsize-1
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Compare_and_swap( base(2), comparand, result_buf(i+1), MPI_INTEGER, i, base_adr(i+1), win, mpierr )
          else
            call foMPI_Compare_and_swap( base(2), comparand, result_buf(i+1), MPI_INTEGER, i, start, win, mpierr )
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 84, rank)
        enddo
    
        call foMPI_Win_complete(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 88, rank)
    
        do i=1,commsize-1
          if ( result_buf(i+1) .ne. mod(i,2) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 91, ": element ", i+1, " is ", result_buf(i+1), &
              " instead of ", mod(i,2), "."
            STOP
          endif
        enddo
        
        if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
          deallocate( base_adr, stat=mpierr )
        endif
        deallocate( result_buf, stat=mpierr )
      else
        call foMPI_Win_post(newgroup, 0, win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 105, rank)
    
        call foMPI_Win_wait(win, mpierr)
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 108, rank)
      
        if ( base(1) .ne. 1 ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 113, ": element ", 1, " is ", base(1), &
            " instead of ", 1, "."
          STOP
        endif

      endif

      call MPI_Group_free( newgroup, mpierr )
 
      call MPI_Barrier( comm, mpierr )

      if (rank == 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_cswap_pscw
