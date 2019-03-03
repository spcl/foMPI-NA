! test needs 15 elements, so local_DIM should be at least 5

      subroutine test_mpi3rma_ddt_1( base, array_size, win, comm ) 

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
      integer :: rank;
      integer :: commsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: start

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      integer(kind=MPI_ADDRESS_KIND) :: lb, extent

      integer, dimension(:), allocatable :: send_buffer
      integer, parameter :: local_DIM = 5

      integer :: temp_dtype
      integer :: origin_dtype
      integer :: target_dtype
      integer ::  type_size

      character(50) :: filename

      write (filename,*) "ddt_1.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      allocate( send_buffer(3*local_DIM), stat=mpierr )

      do i=1,3*local_DIM
        send_buffer(i) = 3*local_DIM*rank+i
        base(i) = -1
      enddo
  
      call MPI_Type_contiguous(3, MPI_INTEGER, temp_dtype, mpierr)
      call MPI_Type_size(MPI_INTEGER, type_size, mpierr)
      lb = 0
      extent = 4*type_size
      call MPI_Type_create_resized( temp_dtype, lb, extent, origin_dtype, mpierr )
      call MPI_Type_commit( origin_dtype, mpierr )
      call foMPI_Type_free( temp_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      call MPI_Type_vector( 3, 4, 5, MPI_INTEGER, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 63, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        call foMPI_Get_address( base(1), start, mpierr )
        call MPI_Sendrecv_replace( start, 1, MPI_AINT, commsize - rank - 1, 0, commsize - rank - 1, 0, comm, MPI_STATUS_IGNORE, &
          mpierr )
      else
        start = 0
      endif

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 77, rank)

      call foMPI_Put( send_buffer(1), 4, origin_dtype, commsize - rank - 1, start, 1, target_dtype, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 80, rank)

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 83, rank)

      call foMPI_Type_free( origin_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 86, rank)
      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 88, rank)

      do k=0,2
        do i=0,2-k
          if( base(local_DIM*k+i+1) .ne. 3*local_DIM*(commsize-rank-1)+local_DIM*k+i+1 ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 92, ": element ", local_DIM*k+i+1, " is ", base(local_DIM*k+i+1), &
              " instead of ", 3*local_DIM*(commsize-rank-1)+local_DIM*k+i, "."
            STOP
          endif
        enddo
        do i=0,k
          if( base(4+k*4+i) .ne. 3*local_DIM*(commsize-rank-1)+(k+1)*local_DIM-k+i ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 100, ": element ", 4+k*4+i, " is ", base(4+k*4+i), &
              " instead of ", &
              3*local_DIM*(commsize-rank-1)+(k+1)*local_DIM-k+i, "."
            STOP
          endif
        enddo
        if( base((k+1)*local_DIM) .ne. -1) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 108, ": element ", (k+1)*local_DIM-1, " is ", base((k+1)*local_DIM-1), &
            " instead of ", -1, "."
          STOP
        endif
      enddo

      deallocate( send_buffer, stat=mpierr )

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then 
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_ddt_1
