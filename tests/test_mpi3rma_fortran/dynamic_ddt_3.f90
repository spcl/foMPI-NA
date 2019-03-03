      subroutine test_mpi3rma_dynamic_ddt_3( base, array_size, DIM1, win, comm )

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

      integer :: target_dtype

      integer, dimension(:), allocatable :: send_buffer

      character(50) :: filename

      write (filename,*) "dynamic_ddt_3.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      allocate( send_buffer(3*DIM1), stat=mpierr )

      do i=1,3*DIM1
        send_buffer(i) = 3*DIM1*rank+i
        base(i) = -1
      enddo
  
      call foMPI_Get_address( base(1), start, mpierr )
      call MPI_Sendrecv_replace( start, 1, MPI_AINT, commsize-rank-1, 0, commsize-rank-1, 0, comm, MPI_STATUS_IGNORE, mpierr )

      call MPI_Type_contiguous( 3*DIM1-1, MPI_INTEGER, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)

      call foMPI_Put( send_buffer(1), 3*DIM1-1, MPI_INTEGER, commsize-rank-1, start, 1, target_dtype, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 49, rank)

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 52, rank)

      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 55, rank)

      do i=1,3*DIM1-1
        if( base(i) .ne. 3*DIM1*(commsize-rank-1)+i ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 58, ": element ", i, " is ", base(i), &
            " instead of ", 3*DIM1*(commsize-rank-1)+i, "."
          STOP
        endif
      enddo
      if( base(3*DIM1) .ne. -1 ) then
        write (*,*) "Error on rank ", rank, " in ", trim(filename), &
          " line ", 65, ": element ", 3*DIM1, " is ", base(3*DIM1), &
          " instead of ", -1, "."
        STOP
      endif

      deallocate( send_buffer, stat=mpierr )

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_dynamic_ddt_3
