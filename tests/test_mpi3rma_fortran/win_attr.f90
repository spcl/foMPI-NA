      subroutine test_mpi3rma_win_attr( comm )

      use ,intrinsic :: iso_c_binding
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

      character(20) :: create_flavor
      integer :: win
      integer :: rank
      integer :: j
      integer :: mpierr

      character, dimension(6) :: space

      integer(kind=MPI_ADDRESS_KIND) :: win_size

      integer(kind=MPI_ADDRESS_KIND) :: argument
      type(c_ptr) :: ptr
      character, dimension(:), pointer :: base
      integer, dimension(1) :: shape

      logical :: flag

      character(50) :: filename

      write (filename,*) "win_attr.f90"

      call MPI_Comm_rank( comm, rank, mpierr )

      do j=0,2
        select case (j)
          case (0)
            win_size = 6
            call foMPI_Win_create( space(1), win_size, 3, MPI_INFO_NULL, comm, win, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 30, rank)
            write (create_flavor,*) "create"

            call foMPI_Win_get_attr( win, foMPI_WIN_BASE, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 34, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Base address retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

!              if ( return_ptr != &space[0] ) {
!                fprintf(stderr, "Error: Base address retrieved by MPI_Win_get_attr is %p instead of %p for MPI_Win_%s.\n", return_ptr, &space[0], create_flavor);
!                exit(EXIT_FAILURE);
!              }

            call foMPI_Win_get_attr( win, foMPI_WIN_SIZE, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 47, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win size retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. 6 ) then
              write (*,*) "Error: Window size retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", 6, &
                " for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_DISP_UNIT, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 61, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win disp retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. 3 ) then
              write (*,*) "Error: Window displacement unit retrieved by", &
                " MPI_Win_get_attr is ", argument, " instead of 3 for ", &
                "MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 76, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win create flavor retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. foMPI_WIN_FLAVOR_CREATE ) then
              write (*,*) "Error: Window create flavor retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                foMPI_WIN_FLAVOR_CREATE, " for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_MODEL, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 91, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win model retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. foMPI_WIN_UNIFIED ) then
              write (*,*) "Error: Window memory model retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                foMPI_WIN_UNIFIED, " for MPI_Win_", create_flavor
              STOP
            endif

          case (1)
            win_size = 15
            call foMPI_Win_allocate( win_size, 5, MPI_INFO_NULL, comm, ptr, win, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 107, rank)
            shape = 15
            call c_f_pointer( ptr, base, shape )
            write (create_flavor,*) "allocate"
 
            call foMPI_Win_get_attr( win, foMPI_WIN_BASE, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 114, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win base retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

!              if ( return_ptr != ptr ) {
!                fprintf(stderr, "Error: Base address retrieved by MPI_Win_get_attr is %p instead of %p for MPI_Win_%s.\n", return_ptr, ptr, create_flavor);
!                exit(EXIT_FAILURE);
!              }

            call foMPI_Win_get_attr( win, foMPI_WIN_SIZE, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 128, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win size retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif
              

            if ( argument .ne. 15 ) then
              write (*,*) "Error: Window size retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", 15, &
                " for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_DISP_UNIT, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 143, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win disp retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. 5 ) then
              write (*,*) "Error: Window displacement unit retrieved ", &
                "by MPI_Win_get_attr is ", argument, &
                " instead of 5 for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 157, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win create flavor retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. foMPI_WIN_FLAVOR_ALLOCATE ) then
              write (*,*) "Error: Window create flavor retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                foMPI_WIN_FLAVOR_ALLOCATE, " for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_MODEL, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 171, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win model retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. foMPI_WIN_UNIFIED ) then
              write (*,*) "Error: Window memory model retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                foMPI_WIN_UNIFIED, " for MPI_Win_", create_flavor
              STOP
            endif

          case (2)
            call foMPI_Win_create_dynamic( MPI_INFO_NULL, comm, win, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 186, rank)  
            write (create_flavor, *) "create_dynamic"

            call foMPI_Win_get_attr( win, foMPI_WIN_BASE, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 190, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win base address retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. MPI_BOTTOM ) then
              write (*,*) "Error: Base address retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                MPI_BOTTOM, " for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_SIZE, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 204, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win size retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif
  
            if ( argument .ne. 0 ) then
              write (*,*) "Error: Window size retrieved by ", &
                "MPI_Win_get_attr is ", argument, &
                " instead of 0 for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_DISP_UNIT, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 218, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win disp retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif
  
            if ( argument .ne. 1 ) then
              write (*,*) "Error: Window displacement unit retrieved ", &
                "by MPI_Win_get_attr is ", argument, &
                " instead of 1 for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_CREATE_FLAVOR, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 232, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win flavor retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. foMPI_WIN_FLAVOR_DYNAMIC ) then
              write (*,*) "Error: Window create flavor retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                foMPI_WIN_FLAVOR_DYNAMIC, " for MPI_Win_", create_flavor
              STOP
            endif

            call foMPI_Win_get_attr( win, foMPI_WIN_MODEL, argument, flag, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 246, rank)
            if (flag .neqv. .true.) then
              write (*,*) "Error: Win model retrieved by ", &
                "MPI_Win_get_attr ended with a false flag.", create_flavor
              STOP
            endif

            if ( argument .ne. foMPI_WIN_UNIFIED ) then
              write (*,*) "Error: Window memory model retrieved by ", &
                "MPI_Win_get_attr is ", argument, " instead of ", &
                foMPI_WIN_UNIFIED, " for MPI_Win_", create_flavor
              STOP
            endif

        end select

        call foMPI_Win_free( win, mpierr )
        call check_fompi_status(mpierr, MPI_SUCCESS, filename, 261, rank)
      enddo

      write (*,*) "No Error"

      end subroutine test_mpi3rma_win_attr
