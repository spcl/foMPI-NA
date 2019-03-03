      subroutine test_mpi3rma_acc_sum_lock( base, array_size, DIM1, win, comm )

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

      write (filename,*) "acc_sum_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 32, rank)

      do i=1,DIM1
        base(i) = rank+i
      enddo
      if (rank .eq. 0) then
        do i=1,DIM1
          base(DIM1+i) = 0
        enddo
      endif

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 44, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 47, rank)
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if ( rank .eq. 0 ) then
          call foMPI_Get_address( base(DIM1+1), start, mpierr)
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr ) ! the init phase is finished
      else
        start = DIM1
        call MPI_Barrier( comm, mpierr ) ! the init phase is finished
      endif

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, 0, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 60, rank)

      call foMPI_Accumulate( base(1), DIM1, MPI_INTEGER, 0, start, DIM1, MPI_INTEGER, foMPI_SUM, win, mpierr)
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 63, rank)

      call foMPI_Win_unlock( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 66, rank)

      call MPI_Barrier( comm, mpierr ) ! wait to finish all accesses

      if ( rank .eq. 0 ) then
        do i=1,DIM1
          if( base(DIM1+i) .ne. ((commsize-1)*((commsize-1)+1)/2+i*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 72, ": element ", i, " is ", base(DIM1+i), &
              " instead of ", (commsize-1)*((commsize-1)+1)/2+i*commsize, "."
            STOP
          endif
        enddo
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

     end subroutine test_mpi3rma_acc_sum_lock 
