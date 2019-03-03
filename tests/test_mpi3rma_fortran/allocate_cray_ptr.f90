      subroutine test_mpi3rma_allocate_cray_ptr( comm )

      use utilities, only: check_fompi_status

#if INCLUDE_TYPE==1
      implicit none
      
      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi
      
      implicit none
#endif

      integer, intent(in) :: comm

      integer :: commsize
      integer :: rank
      integer :: mpierr

      integer, parameter :: array_size = 15

      integer :: win
      integer(kind=MPI_ADDRESS_KIND) :: win_size
      integer(kind=MPI_ADDRESS_KIND) :: start = 0
      integer :: base
      pointer( ptr, base(array_size) )
      
      integer :: i

      character(50) :: filename

      write (filename,*) "allocate_cray_ptr.f90"

      call MPI_Comm_rank( comm, rank, mpierr )
      call MPI_Comm_size( comm, commsize, mpierr )

      ! assume integer has a type size of 4
      win_size = array_size * 4
      call foMPI_Win_allocate( win_size, 4, MPI_INFO_NULL, comm, ptr, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35,  rank)

      do i=1,array_size
        base(i) = 1
      enddo

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 42, rank)

      if( rank .ne. 0 ) then
        call foMPI_Accumulate( base(1), array_size, MPI_INTEGER, 0, start, array_size, MPI_INTEGER, foMPI_SUM, win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)
      endif

      call foMPI_Win_fence( 0, win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)

      if( rank .eq. 0 ) then
        do i=1,array_size
          if(base(i) .ne. commsize) then
            write (*,*) "Error on rank ", rank, " in ", trim(filename), &
              " line ", 54, ": element ", i, " is ", base(i), &
              " instead of ", commsize, "."
            STOP
          endif
        enddo
      endif
 
      call foMPI_Win_free( win, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 64, rank)

      call MPI_Barrier( comm, mpierr )
  
      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

     end subroutine test_mpi3rma_allocate_cray_ptr
