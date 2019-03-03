      subroutine test_mpi3rma_dynamic_ddt_6( base, array_size, DIM1, win, comm )

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

      character(50) :: filename

      write (filename,*) "dynamic_ddt_6.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      do i=0,DIM1-1
        base(3+i) = rank+i
      enddo
      if (rank .eq. 0) then
        do i=0,DIM1-1
          base(DIM1+3+i) = 0
        enddo
      endif

      call MPI_Type_contiguous( DIM1, MPI_INTEGER, target_dtype, mpierr )
      call MPI_Type_commit( target_dtype, mpierr )

      call foMPI_Get_address( base(DIM1+3), start, mpierr )
      call MPI_Bcast( start, 1, MPI_AINT, 0, comm, mpierr )

      call foMPI_Win_fence( foMPI_MODE_NOPRECEDE, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)

      call foMPI_Accumulate( base(3), DIM1, MPI_INTEGER, 0, start, 1, target_dtype, foMPI_SUM, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 49, rank)
  
      call foMPI_Win_fence( foMPI_MODE_NOSUCCEED, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 52, rank)

      call foMPI_Type_free( target_dtype, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 55, rank)

      if ( rank .eq. 0 ) then
        do i=0,DIM1-1 
          if( base(DIM1+3+i) .ne. ((commsize-1)*commsize/2+i*commsize) ) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 59, ": element ", DIM1+3+i, " is ", base(DIM1+3+i), &
              " instead of ", (commsize-1)*commsize/2+i*commsize, "."
            STOP
          endif
        enddo
      endif

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_dynamic_ddt_6
