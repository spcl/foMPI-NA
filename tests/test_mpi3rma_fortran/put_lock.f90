      subroutine test_mpi3rma_put_lock( base, array_size, DIM1, win, comm )

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

      integer ::  minimum

      character(50) :: filename

      write (filename,*) "put_lock.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      if (rank .eq. 0 ) then
        call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, 0, 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35, rank)

        do i=1,DIM1
          base(i) = 2*commsize+i
          base(DIM1+i) = 2*commsize+i
          base(2*DIM1+i) = 2*commsize+i
        enddo

        call foMPI_Win_unlock( 0, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 44, rank)
      endif

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 48, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
 
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0 ) then
          call foMPI_Get_address( base(1), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr ) ! also the signal that the initialization is finished
        start = start + rank * 4 ! assuming integer has a size of 4
      else
        start = rank
        call MPI_Barrier( comm, mpierr ) ! guarantees that the initialization is finished
      endif

      call foMPI_Win_lock( foMPI_LOCK_EXCLUSIVE, 0, 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 66, rank)
  
      if (rank .lt. 3*DIM1) then
        call foMPI_Put( commsize, 1, MPI_INTEGER, 0, start, 1, MPI_INTEGER, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 70, rank)
      endif
  
      call foMPI_Win_unlock( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 74, rank)

      call MPI_Barrier( comm, mpierr ) ! wait for the end of the accesses

      if (rank .eq. 0) then
        if ( 3*DIM1 .gt. commsize ) then
          minimum = commsize
        else
          minimum = 3*DIM1
        endif
        do i=1,minimum
          if( base(i) .ne. commsize ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 85, ": element ", i, " is ", base(i), &
              " instead of ", commsize, "."
            STOP
          endif
        enddo
        if (minimum .lt. 3*DIM1) then
          do i=minimum+1,3*DIM1
            if( base(i) .ne. 2*commsize+mod(i-1,DIM1)+1) then
              write (*,*) "Error on rank ", rank, " in ", trim(filename), &
                " line ", 94, ": element ", i, " is ", base(i), &
                " instead of ", 2*commsize+mod(i-1,DIM1)+1, "."
              STOP
            endif
          enddo
        endif
      endif
  
      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_put_lock
