      subroutine test_mpi3rma_get_acc_replace_lock( base, array_size, DIM1, win, comm )

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

      integer, dimension(:), allocatable :: recv_buf

      character(50) :: filename

      write (filename,*) "get_acc_replace_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 34, rank)

      do i=1,DIM1
        base(i) = rank+i
        base(DIM1+i) = commsize+rank+i
      enddo

      call foMPI_Win_unlock( rank, win, mpierr );
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 42, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 45, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        call foMPI_Get_address( base(DIM1+1), start, mpierr)
        call MPI_Sendrecv_replace( start, 1, MPI_AINT, commsize-rank-1, 0, commsize-rank-1, 0, comm, MPI_STATUS_IGNORE, mpierr ) ! ends the init phase
      else
        start = DIM1
        call MPI_Barrier( comm, mpierr ) ! ends the init phase
      endif

      allocate( recv_buf(DIM1), stat=mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, commsize-rank-1, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 62, rank)
  
      call foMPI_Get_accumulate( base(1), DIM1, MPI_INTEGER, recv_buf(1), DIM1, MPI_INTEGER, commsize-rank-1, start, DIM1, &
        MPI_INTEGER, foMPI_REPLACE, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 65, rank)
  
      call foMPI_Win_unlock( commsize-rank-1, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 68, rank)

      call MPI_Barrier( comm, mpierr ) ! wait for the end of all accesses

      do i=1,DIM1
        if( base(DIM1+i) .ne. (commsize-rank-1+i) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 75, ": element ", DIM1+i, " is ", base(DIM1+i), &
            " instead of ", (commsize-rank-1+i), "."
          STOP
        endif
        if( recv_buf(i) .ne. (commsize+(commsize-rank-1)+i) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 79, ": element ", i, " is ", recv_buf(i), &
            " instead of ", commsize+(commsize-rank-1)+i, "."
          STOP
        endif
      enddo

      deallocate( recv_buf, stat=mpierr )

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_get_acc_replace_lock
