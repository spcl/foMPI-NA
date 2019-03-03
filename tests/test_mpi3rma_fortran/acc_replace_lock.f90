      subroutine test_mpi3rma_acc_replace_lock( base, array_size, DIM1, win, comm )

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

      integer(kind=MPI_ADDRESS_KIND), dimension(:), allocatable :: base_adr

      character(50) :: filename

      write (filename,*) "acc_replace_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 34, rank)
  
      do i=1,DIM1
        base(i) = rank+i
      enddo
      if (rank .eq. 0) then
        do i=1,DIM1
          base(DIM1+i) = commsize+i
        enddo
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
        allocate( base_adr(commsize), stat=mpierr )
        call foMPI_Get_address( base(1), start, mpierr )
        call MPI_Allgather( start, 1, MPI_AINT, base_adr(1), 1, MPI_AINT, comm, mpierr ) ! the init phase is finished
      else
        start = 0
        call MPI_Barrier( comm, mpierr ) ! the init phase is finished
      endif

      if (rank .eq. 0) then
        do i=0,commsize-1
          call foMPI_Win_lock( foMPI_LOCK_SHARED, i, 0, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 67, rank)
      
          if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
            call foMPI_Accumulate( base(DIM1+1), DIM1, MPI_INTEGER, i, base_adr(i+1), DIM1, MPI_INTEGER, foMPI_REPLACE, &
              win, mpierr )
          else
            call foMPI_Accumulate( base(DIM1+1), DIM1, MPI_INT, i, start, DIM1, MPI_INTEGER, foMPI_REPLACE, win, mpierr )
          endif
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)
      
          call foMPI_Win_unlock( i, win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 77, rank)
        enddo
      endif

      call MPI_Barrier( comm, mpierr ) !wait for all accesses to finish

      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        deallocate( base_adr, stat=mpierr )
      endif

      do i=1,DIM1
        if( base(i) .ne. (commsize+i) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 88, ": element ", i, " is ", base(i), &
            " instead of ", commsize+i, "."
          STOP
        endif
      enddo

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_acc_replace_lock
