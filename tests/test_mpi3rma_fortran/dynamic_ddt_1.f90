      subroutine test_mpi3rma_dynamic_ddt_1( base, array_size, win, comm )

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

      integer :: local_DIM = 5
      integer, dimension(:), allocatable :: send_buffer

      integer :: temp_dtype
      integer :: target_dtype

      character(50) :: filename

      write (filename,*) "dynamic_ddt_1.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      allocate( send_buffer( 3 * local_DIM), stat=mpierr )

      do i=0,3*local_DIM-1
        send_buffer(i+1) = 3*local_DIM*rank+i
        base(i+1) = -1
      enddo
  
      call MPI_Type_vector( 2, 2, 3, MPI_INTEGER, temp_dtype, mpierr )
      call MPI_Type_vector( 2, 1, 2, temp_dtype, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )
      call foMPI_Type_free( temp_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 42, rank)

      call foMPI_Get_address( base(1), start, mpierr )
      call MPI_Sendrecv_replace( start, 1, MPI_AINT, commsize-rank-1, 0, commsize-rank-1, 0, comm, MPI_STATUS_IGNORE, mpierr )

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 48, rank)

      call foMPI_Put( send_buffer(1), 8, MPI_INTEGER, commsize-rank-1, start, 1, target_dtype, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 51, rank)

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 54, rank)

      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      do k=0,1
        do i=0,1
          if( base(2*local_DIM*k+i+1) .ne. 3*local_DIM*(commsize-rank-1)+4*k+i ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 61, ": element ", 2*local_DIM*k+i+1, " is ", base(2*local_DIM*k+i+1), &
              " instead of ", 3*local_DIM*(commsize-rank-1)+4*k+i, "."
            STOP
          endif
        enddo
        do i=0,1
          if( base(2*local_DIM*k+i+4) .ne. 3*local_DIM*(commsize-rank-1)+k*4+2+i ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 69, ": element ", 2*local_DIM*k+i+1, " is ", base(2*local_DIM*k+i+4), &
              " instead of ", 3*local_DIM*(commsize-rank-1)+k*4+2+i, "."
            STOP
          endif
        enddo
        if( base(k*2*local_DIM+3) .ne. -1) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 76, ": element ", k*2*local_DIM+3, " is ", base(k*2*local_DIM+3), &
            " instead of ", -1, "."
          STOP
        endif
      enddo
      do i=local_DIM,2*local_DIM-1
        if(base(i+1) .ne. -1) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 84, ": element ", i+1, " is ", base(i+1), &
            " instead of ", -1, "."
          STOP
        endif
      enddo

      deallocate( send_buffer, stat=mpierr )

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

     end subroutine test_mpi3rma_dynamic_ddt_1
