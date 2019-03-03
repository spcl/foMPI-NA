      subroutine test_mpi3rma_dynamic_ddt_5( base, array_size, win, comm )

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

      integer :: target_dtype
      integer :: origin_dtype
      integer, dimension(3) :: blocklength
      integer, dimension(3) :: displacement

      character(50) :: filename

      write (filename,*) "dynamic_ddt_5.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      if (rank == 0) then
        do i=4,6
          base(i) = 0;
        enddo
        base(9) = 0
        do i=12,14
          base(i) = 0
        enddo
      endif
      do i=0,2 
        base(i+1) = rank+i
      enddo
      do i=0,1
        base(7+i) = rank+i+3
      enddo
      do i=0,1
        base(10+i) = rank+i+5
      enddo
 
      blocklength(1) = 3
      blocklength(2) = 1
      blocklength(3) = 3
      displacement(1) = 3
      displacement(2) = 8
      displacement(3) = 11

      call MPI_Type_indexed( 3, blocklength(1), displacement(1), MPI_INTEGER, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )

      blocklength(1) = 3
      blocklength(2) = 2
      blocklength(3) = 2
      displacement(1) = 0
      displacement(2) = 6
      displacement(3) = 9

      call MPI_Type_indexed( 3, blocklength(1), displacement(1), MPI_INTEGER, origin_dtype, mpierr )
      call MPI_Type_commit( origin_dtype, mpierr )

      call foMPI_Get_address( base(1), start, mpierr )
      call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )

      call foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 77, rank)

      call foMPI_Accumulate( base(1), 1, origin_dtype, 0, start, 1, target_dtype, foMPI_SUM, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 80, rank)
 
      call foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 83, rank)

      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 86, rank)
      call foMPI_Type_free( origin_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 88, rank)

      if ( rank .eq. 0 ) then
        do k=0,2
          if( base(4+k) .ne. ((commsize-1)*commsize/2+k*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 92, ": element ", 4+k, " is ", base(4+k), &
              " instead of ", (commsize-1)*commsize/2+k*commsize, "."
            STOP
          endif
        enddo
        if( base(9) .ne. ((commsize-1)*commsize/2+3*commsize) ) then
          write (*,*) "Error on rank ", rank, " in ", trim(filename), &
            " line ", 99, ": element ", 49, " is ", base(9), &
            " instead of ", (commsize-1)*commsize/2+3*commsize, "."
          STOP
        endif
        do k=0,2
          if( base(12+k) .ne. ((commsize-1)*commsize/2+(4+k)*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 106, ": element ", 12+k, " is ", base(12+k), &
              " instead of ", (commsize-1)*commsize/2+(4+k)*commsize, &
              "."
            STOP
          endif
        enddo
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

     end subroutine test_mpi3rma_dynamic_ddt_5
