      subroutine test_mpi3rma_get_acc_sum_lock( base, array_size, DIM1, win, comm )

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

      write (filename,*) "get_acc_sum_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      allocate( recv_buf(DIM1), stat=mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 36, rank)

      do i=1,DIM1
        base(i) = i
      enddo
      if (rank .eq. 0) then
        do i=1,DIM1
          base(DIM1+i) = i
        enddo
      endif

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 48, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 51, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif 
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if ( rank == 0 ) then
          call foMPI_Get_address( base(DIM1+1), start, mpierr)
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr ) ! ends the init phase
      else
        start = DIM1
        call MPI_Barrier( comm, mpierr ) ! ends the init phase
      endif

      call foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 68, rank)
  
      call foMPI_Get_accumulate( base(1), DIM1, MPI_INTEGER, recv_buf(1), DIM1, MPI_INTEGER, 0, start, DIM1, MPI_INTEGER, &
        foMPI_SUM, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 71, rank)

      call foMPI_Win_unlock( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)

      call MPI_Barrier( comm, mpierr ) ! wait for all accesses to finish

      if ( rank .eq. 0 ) then
        do i=1,DIM1
          if( base(DIM1+i) .ne. (commsize+1)*i ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 80, ": element ", DIM1+i, " is ", base(DIM1+i), &
              " instead of ", (commsize+1)*i, "."
            STOP
          endif
        enddo
      endif
      do i=1,DIM1
        if( mod(recv_buf(i),i) .ne. 0) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 89, ": element ", i, " is ", mod(recv_buf(i),i), &
            " instead of ", 0, "."
          STOP
        endif
      enddo

      call MPI_Barrier( comm, mpierr )

      if (rank .eq. 0) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_get_acc_sum_lock
