      subroutine test_mpi3rma_get_acc_maxloc_lock( base, array_size, DIM1, win, comm )

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
      real, dimension(2,array_size), intent(inout) :: base
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

      real, dimension(2,array_size) :: result_buf

      real, parameter :: fompi_epsilon = 0.00000000001

      character(50) :: filename

      write (filename,*) "get_acc_maxloc_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, rank, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 34, rank)

      if ( ((commsize/2) .eq. rank) .or. ((commsize/2+1) .eq. rank) ) then
        do i=1,DIM1
          base(1,i) = real(commsize*2+i)
          base(2,i) = real(rank)
        enddo
      else
        do i=1,DIM1
          base(1,i) = real(commsize+rank+i)
          base(2,i) = real(rank)
        enddo
      endif

      call foMPI_Win_unlock( rank, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 49, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 52, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
 
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0 ) then
          call foMPI_Get_address( base(1,1), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr ) ! also the signal that the initialization is finished
      else
        start = 0
        call MPI_Barrier( comm, mpierr ) ! guarantees that the initialization is finished
      endif

      if (rank .ne. 0 ) then
        call foMPI_Win_lock( foMPI_LOCK_SHARED, 0, 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 70, rank)
     
        call foMPI_Get_accumulate( base(1,1), DIM1, MPI_2REAL, result_buf(1,1), DIM1, MPI_2REAL, 0, start, DIM1, MPI_2REAL, &
          foMPI_MAXLOC, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 73, rank)
      
        call foMPI_Win_unlock( 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 76, rank)
      endif
  
      call MPI_Barrier( comm, mpierr ) ! wait for the end of the accesses

      if (rank .eq. 0) then
        do i=1,DIM1
          if ( (base(1,i) - real(commsize*2+i) .gt. fompi_epsilon) .or. (int(base(2,i)) .ne. (commsize/2)) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 83, ": max element ", i, " is ", base(1,i), &
              " instead of ", real(commsize*2+i), " or index is ", int(base(2,i)), &
              " instead of ", commsize/2, "."
            STOP
          endif
        enddo
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_get_acc_maxloc_lock
