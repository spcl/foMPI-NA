      subroutine test_mpi3rma_ddt_2( base, array_size, win, comm )

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

      integer :: origin_dtype
      integer :: target_dtype

      character(50) :: filename

      write (filename,*) "ddt_2.f90"
  
      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      do i=1,7
        base(i) = rank+i
      enddo
      if (rank .eq. 0) then
        do i=1,8
          base(7+i) = 0
        enddo
      endif

      call MPI_Type_vector(2, 3, 4, MPI_INTEGER, origin_dtype, mpierr)
      call MPI_Type_commit(origin_dtype, mpierr)

      call MPI_Type_vector(3, 2, 3, MPI_INTEGER, target_dtype, mpierr)
      call MPI_Type_commit(target_dtype, mpierr)

      call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, create_flavor, flag, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)
      if (flag .neqv. .true.) then
        write (*,*) "Didn't found foMPI_WIN_CREATE_FLAVOR keyval."
        STOP
      endif 
  
      if ( create_flavor .eq. foMPI_WIN_FLAVOR_DYNAMIC ) then
        if (rank .eq. 0) then
          call foMPI_Get_address( base(8), start, mpierr )
        endif
        call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )
      else
        start = 7
      endif

      call foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 66, rank)

      call foMPI_Accumulate( base(1), 1, origin_dtype, 0, start, 1, target_dtype, foMPI_SUM, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 69, rank)
 
      call foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 72, rank)

      call foMPI_Type_free( origin_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 75, rank)
      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 77, rank)

      if ( rank .eq. 0 ) then
        do k=1,2
          if( base(7+k) .ne. ((commsize-1)*commsize/2+k*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 81, ": element ", 7+k, " is ", base(7+k), &
              " instead of ", (commsize-1)*commsize/2+k*commsize, "."
            STOP
          endif
        enddo
        if( base(11) .ne. ((commsize-1)*commsize/2+3*commsize) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 88, ": element ", 11, " is ", base(11), &
            " instead of ", (commsize-1)*commsize/2+2*commsize, "."
          STOP
        endif
        if( base(12) .ne. ((commsize-1)*commsize/2+5*commsize) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 94, ": element ", 12, " is ", base(12), &
            " instead of ", (commsize-1)*commsize/2+4*commsize, "."
          STOP
        endif
        do k=1,2
          if( base(13+k) .ne. ((commsize-1)*commsize/2+(5+k)*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 101, ": element ", 13+k, " is ", base(13+k), &
              " instead of ", ((commsize-1)*commsize/2+(5+k)*commsize), "."
            STOP
          endif
        enddo
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_ddt_2
