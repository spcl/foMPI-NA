      subroutine test_mpi3rma_dynamic_attach_2( DIM1, win, comm )

      use utilities, only: check_fompi_status

#if INCLUDE_TYPE==1
      implicit none
      
      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi
      
      implicit none
#endif

      integer, intent(in) :: DIM1
      integer, intent(in) :: win
      integer, intent(in) :: comm

      integer :: rank
      integer :: mpierr

      integer(kind=MPI_ADDRESS_KIND) :: attach_size

      integer, dimension(:), allocatable :: base
      integer, dimension(:), allocatable :: base2
      integer, dimension(:), allocatable :: base3

      character(50) :: filename

      write (filename,*) "dynamic_attach_2.f90"

      call MPI_Comm_rank( comm, rank, mpierr )

      allocate( base(DIM1), stat=mpierr )
      allocate( base2(3*DIM1), stat=mpierr )
      allocate( base3(DIM1), stat=mpierr )

      ! assuming an integer size of 4
      attach_size = DIM1*4
      call foMPI_Win_attach( win, base(1), attach_size, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 35, rank)

      attach_size = 3*DIM1*4
      call foMPI_Win_attach( win, base2(1), attach_size, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 39, rank)

      attach_size = DIM1*4
      call foMPI_Win_attach( win, base3(1), attach_size, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 43, rank)

      call foMPI_Win_detach( win, base2(1), mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 46, rank)

      attach_size = 2*DIM1*4
      call foMPI_Win_attach( win, base2(1), attach_size, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 50, rank)

      attach_size = DIM1*4
      call foMPI_Win_attach( win, base2(2*DIM1+1), attach_size, mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 54, rank)

      call foMPI_Win_detach( win, base3(1), mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 57, rank)

      call foMPI_Win_detach( win, base2(1), mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 60, rank)

      call foMPI_Win_detach( win, base(1), mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 63, rank)

      call foMPI_Win_detach( win, base2(2*DIM1+1), mpierr )
      call check_fompi_status(mpierr, MPI_SUCCESS, filename, 66, rank)

      deallocate( base, stat=mpierr )
      deallocate( base2, stat=mpierr )
      deallocate( base3, stat=mpierr )

      call MPI_Barrier( comm, mpierr )

      if ( rank .eq. 0 ) then
        write (*,*) "No Error"
      endif

      end subroutine test_mpi3rma_dynamic_attach_2
