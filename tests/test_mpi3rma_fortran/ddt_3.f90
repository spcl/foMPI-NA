      subroutine test_mpi3rma_ddt_3( base, array_size, win, comm )

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

      logical :: flag
      integer(kind=MPI_ADDRESS_KIND) :: create_flavor

      integer(kind=MPI_ADDRESS_KIND) :: lb, extent

      integer :: origin_dtype
      integer :: target_dtype
      integer :: result_dtype
      integer :: temp_dtype
      integer :: type_size

      integer, dimension(:), allocatable :: recv_buf

      character(50) :: filename

      write (filename,*) "ddt_3.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      allocate( recv_buf(12), stat=mpierr )

      do k=0,2
        do i=0,1
          base(k*3+i+1) = 2*k+i
        enddo
      enddo
      if (rank .eq. 0) then
        do k=0,1
          do i=0,2
            base(9+4*k+i) = k*3+i
          enddo
        enddo
      endif
      do i=1,12
        recv_buf(i) = 0
      enddo

      call MPI_Type_vector( 3, 2, 3, MPI_INTEGER, origin_dtype, mpierr )
      call MPI_Type_commit( origin_dtype, mpierr )
      call MPI_Type_vector( 6, 1, 2, MPI_INTEGER, result_dtype, mpierr )
      call MPI_Type_commit( result_dtype, mpierr )
      call MPI_Type_vector( 1, 3, 4, MPI_INTEGER, temp_dtype, mpierr )
      call MPI_Type_size( MPI_INTEGER, type_size, mpierr )
      lb = 0
      extent = 4*type_size
      call MPI_Type_create_resized( temp_dtype, lb, extent, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )
      call foMPI_Type_free( temp_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 70, rank)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 73, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif
 
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0) then
          call foMPI_Get_address( base(9), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 8
      endif

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 89, rank)
  
      call foMPI_Get_accumulate( base(1), 1, origin_dtype, recv_buf(1), 1, result_dtype, 0, start, 2, target_dtype, foMPI_SUM, &
        win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 92, rank)
 
      call foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 95, rank)

      call foMPI_Type_free( origin_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 98, rank)
      call foMPI_Type_free( result_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 100, rank)
      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 102, rank)

      if ( rank .eq. 0 ) then
        do k=0,1
          do i=0,2
            if( base(9+k*4+i) .ne. ((commsize+1)*(k*3+i)) ) then
              write (*,*) "Error on rank ", rank, " in ", trim(filename), &
                " line ", 107, ": element ", 9+k*4+i, " is ", base(9+k*4+i), &
                " instead of ", (commsize+1)*(k*3+i), "."
              STOP
            endif
          enddo
        enddo
      endif

      if( recv_buf(1) .ne. 0) then
        write (*,*) "Error on rank ", rank, " in ", trim(filename), &
          " line ", 117, ": element ", 1, " is ", recv_buf(1), &
          " instead of ", 0, "."
        STOP
      endif
      if( recv_buf(2) .ne. 0) then
        write (*,*) "Error on rank ", rank, " in ", trim(filename), &
          " line ", 123, ": element ", 2, " is ", recv_buf(2), &
          " instead of ", 0, "."
        STOP
      endif
      do i=1,5
        if( mod(recv_buf(2*i+1),i) .ne. 0) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 130, ": element ", 2*i+1, " is ", mod(recv_buf(2*i+1),i), &
            " instead of ", 0, "."
          STOP
        endif
        if( recv_buf(2*i+2) .ne. 0) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 136, ": element ", 2*i+2, " is ", recv_buf(2*i+1), &
            " instead of ", 0, "."
          STOP
        endif
      enddo

      deallocate( recv_buf, stat=mpierr )

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_ddt_3
