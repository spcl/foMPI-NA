      subroutine test_mpi3rma_dynamic_ddt_4( base, array_size, win, comm )

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
      integer :: k
      integer :: rank
      integer :: commsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: start

      integer, parameter :: local_DIM = 5

      integer :: target_dtype
      integer, dimension(2) :: blocklength
      integer, dimension(2) :: displacement

      character(50) :: filename

      write (filename,*) "dynamic_ddt_4.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      if (rank .eq. 0) then
        do i=1,3
          base(i) = 0
        enddo
        do i=1,4
          base(2*local_DIM+i) = 0
        enddo
      endif
      do i=0,6
        base(4+i) = rank+i
      enddo
 
      blocklength = (/3, 4/)
      displacement = (/ 0, 2*local_DIM /)

      call MPI_Type_indexed( 2, blocklength(1), displacement(1), MPI_INTEGER, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )

      call foMPI_Get_address( base(1), start, mpierr )
      call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )

      call foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      call foMPI_Accumulate( base(4), 7, MPI_INTEGER, 0, start, 1, target_dtype, foMPI_SUM, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 60, rank)
 
      call foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 63, rank)

      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 66, rank)

      if ( rank .eq. 0 ) then
        do k=0,2
          if( base(k+1) .ne. ((commsize-1)*commsize/2+k*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 72, ": element ", k+1, " is ", base(k+1), &
              " instead of ", (commsize-1)*commsize/2+k*commsize, "."
            STOP
          endif
        enddo
        do k=0,3
          if( base(2*local_DIM+k+1) .ne. ((commsize-1)*commsize/2+(3+k)*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 78, ": element ", 2*local_DIM+k+1, " is ", base(2*local_DIM+k+1), &
              " instead of ", (commsize-1)*commsize/2+(3+k)*commsize, "."
            STOP
          endif
        enddo
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_dynamic_ddt_4
