      subroutine test_mpi3rma_get_pscw( base, array_size, DIM1, win, comm )

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

      integer, dimension(3) :: ranges
      integer :: group
      integer :: newgroup
      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr

      character(50) :: filename

      write (filename,*) "get_pscw.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      ranges(1) = mod(rank+1,2)
      ranges(2) = commsize-1
      ranges(3) = 2
      call MPI_Comm_group(comm, group, mpierr)
      call MPI_Group_range_incl(group, 1, ranges, newgroup, mpierr)
      call MPI_Group_free( group, mpierr )

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 44, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        allocate( base_adr(commsize), stat=mpierr )
        call foMPI_Get_address( base(2*DIM1+1), start, mpierr )
        call MPI_Allgather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, comm, mpierr )
      else
        start = 2*DIM1
      endif

      do i=1,DIM1
        base(i) = 0
        base(2*DIM1+i) = 4*DIM1+i
      enddo

      call foMPI_Win_post(newgroup, 0, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 64, rank)
  
      call foMPI_Win_start(newgroup, 0, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 67, rank)
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        call foMPI_Get( base(1), DIM1, MPI_INTEGER, mod(rank+1,2), base_adr(mod(rank+1,2)+1), DIM1, MPI_INTEGER, win, mpierr )
      else
        call foMPI_Get( base(1), DIM1, MPI_INTEGER, mod(rank+1,2), start, DIM1, MPI_INTEGER, win, mpierr )
      endif
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 75, rank)
  
      call foMPI_Win_complete(win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 79, rank)
  
      call foMPI_Win_wait(win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 82, rank)

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        deallocate( base_adr, stat=mpierr )
      endif
      call MPI_Group_free( newgroup, mpierr )

      do i=1,DIM1
        if( base(i) .ne. 4*DIM1+i ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 90, ": element ", i, " is ", base(i), &
            " instead of ", 4*DIM1+i, "."
          STOP
        endif
      enddo

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

     end subroutine test_mpi3rma_get_pscw 
