      program test_mpi3rma_fortran

      use ,intrinsic :: iso_c_binding
      use utilities, only: check_fompi_status

!#ifndef INCLUDE_TYPE
!#error "HRT_ARCH macro not set"
#if INCLUDE_TYPE==1
      implicit none

      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi

      implicit none
#endif
!#endif

      integer, parameter :: DIM1 = 5

      integer :: j
      integer :: i
      integer :: win
      integer :: win2
      integer :: rank
      integer ::  commsize
      character(20) :: create_flavor
      integer :: test_counter = 0
      
      integer, dimension(:), pointer :: base
      integer, dimension(:), pointer :: base2
      real, dimension(:,:), pointer :: base3
      integer(kind=8), dimension(:), pointer :: base4
      
      integer, dimension(1) :: shape_1d
      integer, dimension(2) :: shape_2d
      type(c_ptr) :: ptr
      
      integer :: comm
      integer :: grank
      integer :: gcommsize
      integer :: mpierr
      integer(kind=MPI_ADDRESS_KIND) :: win_size

      double precision :: time(3)

      character :: newline
      character(50) :: filename

      test_counter = 0
      newline = char(10)
      write (filename,*) "main.f90"

      call foMPI_Init( mpierr )

      call MPI_Comm_rank( MPI_COMM_WORLD, grank, mpierr )
      call MPI_Comm_size( MPI_COMM_WORLD, gcommsize, mpierr )

      if (gcommsize .lt. 4) then
        write (*,*) "The tests need at least four processes. So ", &
          "please rerun it.", newline
        call foMPI_Finalize( mpierr )
        STOP
      endif

!=============================================================================
!========================== NULL pointer test ================================
!=============================================================================

      ! only work with Cray compiler at the momemt
      !if (grank .eq. 0) then
      !  write (*,*) "test ", test_counter, ": null pointer"
      !  test_counter = test_counter + 1
      !  call test_mpi3rma_null_pointer( MPI_COMM_SELF )
      !endif

!=============================================================================
!======================== Window attributes test =============================
!=============================================================================

      if (grank .eq. 0) then
        write (*,*) "test ", test_counter, ": window attributes"
        test_counter = test_counter + 1
        call test_mpi3rma_win_attr( MPI_COMM_SELF )
      endif

      do i=0,2

        select case (i)
          case (0)
            comm = MPI_COMM_WORLD
          case (1)
            call MPI_Comm_split(MPI_COMM_WORLD, mod(grank,2), 0, comm, mpierr)
          case (2)
            if ( grank .lt. gcommsize-2) then
              call MPI_Comm_split(MPI_COMM_WORLD, 0, 0, comm, mpierr)
            else
              call MPI_Comm_split(MPI_COMM_WORLD, 1, 0, comm, mpierr)
            endif
        end select

        call MPI_Comm_rank( comm, rank, mpierr )
        call MPI_Comm_size( comm, commsize, mpierr )

!=============================================================================
!===================== bookkeeping exclusive locks test ======================
!=============================================================================

        if (rank .eq. 0) then
          write (*,*) "test ", test_counter, ": bookkeeping exclusive ", &
            "locks"
          test_counter = test_counter + 1
        endif

        call test_mpi3rma_excl_lock( comm )

        do j=0,2
          time(j+1) = - MPI_Wtime()

          select case (j)
            case (0)
              allocate( base(3*DIM1), stat=mpierr )
              ! assuming size of an integer is 4
              win_size = DIM1*4
              call foMPI_Win_create(base(6), win_size, 4, MPI_INFO_NULL, MPI_COMM_WORLD, win2, mpierr)
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 116, grank)
              
              win_size = 3*DIM1*4
              call foMPI_Win_create(base(1), win_size, 4, MPI_INFO_NULL, comm, win, mpierr)
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 121, grank)
              
              write (create_flavor,*) "create"
            case (1)
              if (rank .eq. 0) then
                write (*,*) "test ", test_counter, ": Win_allocate with", &
                  " cray pointer"
                test_counter = test_counter + 1
              endif
              call test_mpi3rma_allocate_cray_ptr( comm )
              if (rank .eq. 0) then
                write (*,*) "test ", test_counter, ": Win_allocate with", &
                  " ISO C binding"
                test_counter = test_counter + 1
              endif
              call test_mpi3rma_allocate_c_intrinisic( comm )
              
              ! assuming size of an integer is 4
              win_size = 3*DIM1*4
              call foMPI_Win_allocate( win_size, 4, MPI_INFO_NULL, comm, ptr, win, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 140, grank)
              shape_1d = 3*DIM1
              call c_f_pointer( ptr, base, shape_1d )
          
              ! assuming size of an integer is 4
              win_size = DIM1*4
              call foMPI_Win_allocate( win_size, 4, MPI_INFO_NULL, MPI_COMM_WORLD, ptr, win2, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 147, grank)
              shape_1d = DIM1
              call c_f_pointer( ptr, base2, shape_1d )
              
              write (create_flavor,*) "allocate"
            case (2)
              call foMPI_Win_create_dynamic( MPI_INFO_NULL, MPI_COMM_WORLD, win2, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 154, grank)
              call foMPI_Win_create_dynamic( MPI_INFO_NULL, comm, win, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 156, grank)

              allocate( base(3*DIM1), stat=mpierr )
              ! assuming size of an integer is 4
              win_size = 2*DIM1*4
              call foMPI_Win_attach( win2, base(1), win_size, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 163, grank)

              call MPI_Barrier( MPI_COMM_WORLD, mpierr )

              write (create_flavor,*) "dynamic"
          end select
 
!=============================================================================
!=========================== window name test ================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - win name"
            test_counter = test_counter + 1
            call test_mpi3rma_win_name( win2 )
          endif

          if ( j .eq. 2 ) then
!=============================================================================
!========================= dynamic attach 1 test =============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - attach 1"
              test_counter = test_counter + 1
            endif
            call test_mpi3rma_dynamic_attach_1( DIM1, win, comm )
 
!=============================================================================
!========================= dynamic attach 2 test =============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - attach 2"
              test_counter = test_counter + 1
            endif
            call test_mpi3rma_dynamic_attach_2( DIM1, win, comm )

          endif

!=============================================================================
!============================ put fence test =================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Put (fence)"
            test_counter = test_counter + 1
          endif
          if ( j .eq. 2 ) then
            call test_mpi3rma_put_fence(base, 3*DIM1, DIM1, win2, MPI_COMM_WORLD)
          else
            call test_mpi3rma_put_fence(base, 3*DIM1, DIM1, win, comm)
          endif

!=============================================================================
!============================ get fence test =================================
!=============================================================================

          if ( j .eq. 2 ) then
            call foMPI_Win_detach( win2, base(1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 226, grank)
            ! assuming size of an integer is 4
            win_size = 3*DIM1*4
            call foMPI_Win_attach( win2, base(1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 230, grank)

            call MPI_Barrier( MPI_COMM_WORLD, mpierr )
          endif

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
            " - Get (fence)"
            test_counter = test_counter + 1
          endif
          if ( j .eq. 2 ) then
            call test_mpi3rma_get_fence(base, 3*DIM1, DIM1, win2, MPI_COMM_WORLD)
          else
            call test_mpi3rma_get_fence(base, 3*DIM1, DIM1, win, comm)
          endif

!=============================================================================
!============================ get lock test ==================================
!=============================================================================

          if ( j .eq. 2 ) then
            call foMPI_Win_detach( win2, base(1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 252, grank)
      
            ! assuming an integer size of 4
            win_size = DIM1*4
            call foMPI_Win_attach( win, base(1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 257, grank)
            call foMPI_Win_attach( win, base(DIM1+1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 259, grank)
            call foMPI_Win_attach( win, base(2*DIM1+1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 261, grank)

            ! call MPI_Barrier( comm, mpierr ) since there is already a
            ! barrier afterwards because of the sync change
          endif

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
    
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!============================ put lock test ==================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Put (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_put_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!=========== rput with mpi_test & rget with mpi_wait (lock) test =============
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Rput/Test & Rget/Wait (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_rput_rget_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!==================== rget with mpi_test (lockall) test ======================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Rget/Test (lock_all)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_rget_lockall(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!===================== lock exclusive concurrency test =======================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Exclusive lock concurrency"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_lock_exclusive_concurrency(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!============================ put pscw test ==================================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
 
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Put (PCSW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_put_pscw(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!============================ get pscw test ==================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get (PSCW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_pscw(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!============================== pscw test ====================================
!=============================================================================
! This tests if a non-blocking FOMPI_Win_start is working.
! We choose that FOMPI_Win_start is always blocking, so this test is removed for now.
!
!          if (rank .eq. 0) then
!            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
!             " - PSCW 1"
!            test_counter = test_counter + 1
!          endif
!          call test_mpi3rma_pscw1(win, comm)
!
!=============================================================================
!============================== pscw test ====================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - PSCW 2/Win_test"
            test_counter = test_counter + 1
          endif      
          call test_mpi3rma_pscw2(win, comm)

!=============================================================================
!====================== accumulate sum fence test ============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
    
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Accumulate/SUM (Fence)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_acc_sum_fence(base, 3*DIM1, DIM1, win, comm)
 
!=============================================================================
!====================== accumulate sum pscw test =============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Accumulate/SUM (PSCW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_acc_sum_pscw(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!====================== accumulate sum lock test =============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
    
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Accumulate/SUM (Lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_acc_sum_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!==================== accumulate replace lock test ===========================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Accumulate/REPLACE (Lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_acc_replace_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!====================== raccumulate sum lock test ============================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Raccumulate/SUM (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_racc_sum_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!==================== get_accumulate sum fence test ==========================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/SUM (fence)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_acc_sum_fence(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!==================== get_accumulate sum PSCW test ===========================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/SUM (PSCW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_acc_sum_pscw(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!==================== get_accumulate sum lock test ===========================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/SUM (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_acc_sum_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!================== get_accumulate replace lock test =========================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/REPLACE (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_acc_replace_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!=================== get_accumulate no_op lock test ==========================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/NO_OP (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_acc_no_op_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!=================== rget_accumulate sum lock test ===========================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Rget_accumulate/SUM (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_rget_acc_sum_lock(base, 3*DIM1, DIM1, win, comm)

!=============================================================================
!==================== compare_and_swap fence test ============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
  
          if (rank.eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Compare_and_swap (fence)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_cswap_fence(base, 3*DIM1, win, comm)

!=============================================================================
!==================== compare_and_swap PSCW test =============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
  
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Compare_and_swap (PSCW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_cswap_pscw(base, 3*DIM1, win, comm)

!=============================================================================
!==================== compare_and_swap lock test =============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
  
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Compare_and_swap (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_cswap_lock(base, 3*DIM1, win, comm)

!=============================================================================
!==================== fetch_and_op sum fence test ============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
  
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Fetch_and_op/SUM (fence)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_fetch_and_op_sum_fence(base, 3*DIM1, win, comm)

!=============================================================================
!==================== fetch_and_op sum PSCW test =============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
  
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Fetch_and_op/SUM (PSCW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_fetch_and_op_sum_pscw(base, 3*DIM1, win, comm)

!=============================================================================
!==================== fetch_and_op sum lock test =============================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change
  
          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Fetch_and_op/SUM (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_fetch_and_op_sum_lock(base, 3*DIM1, win, comm)

!=============================================================================
!======================== MPI Datatype test 1 ================================
!=============================================================================

!         change the memory regions for all datatype tests
          if (j .eq. 2) then
            call foMPI_Win_detach( win, base(1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 586, grank)
            call foMPI_Win_detach( win, base(DIM1+1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 588, grank)
            call foMPI_Win_detach( win, base(2*DIM1+1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 590, grank)

            win_size = 3*DIM1*4 ! assuming integer size of 4
            call foMPI_Win_attach( win, base(1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 594, grank)

            call MPI_Barrier( comm, mpierr )
          endif

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Put with DDTs (in one memory region) (fence)"
            test_counter = test_counter + 1
          endif

          if (DIM1 .ge. 5) then
            call test_mpi3rma_ddt_1( base, 3*DIM1, win, comm )
          else
            write (*,*) "Memory region is too small for the test. DIM1",  &
             " has to be at least 5."
            STOP
          endif

!=============================================================================
!======================== MPI Datatype test 2 ================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Accumulate/SUM with DDTs (in one memory region)", &
              " (fence)"
            test_counter = test_counter + 1
          endif
    
          if (DIM1 .ge. 5) then
            call test_mpi3rma_ddt_2( base, 3*DIM1, win, comm )
          else
            write (*,*) "Memory region is too small for the test. DIM1", &
              " has to be at least 5."
            STOP
          endif

!=============================================================================
!======================== MPI Datatype test 3 ================================
!=============================================================================

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/SUM with DDTs (fence)"
            test_counter = test_counter + 1
          endif
    
          if (DIM1 .ge. 5) then
            call test_mpi3rma_ddt_3( base, 3*DIM1, win, comm )
          else
            write (*,*) "Memory region is too small for the test. DIM1", &
              " has to be at least 5."
            STOP
          endif

!         preparations for the next test
          if (j .eq. 2) then
            call foMPI_Win_detach( win, base(1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 655, grank)

            win_size = DIM1*4 ! assuming an integer size of 4
            call foMPI_Win_attach( win, base(1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 659, grank)
            call foMPI_Win_attach( win, base(2*DIM1+1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 661, grank)
      
            call MPI_Barrier( comm, mpierr )
          endif

!=============================================================================
!==================== dynamic MPI Datatype test 1 ============================
!=============================================================================

          if (j .eq. 2) then
            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - Put with DDTs (in more than one memory region)", &
                " (fence)"
              test_counter = test_counter + 1
            endif

            if (DIM1 .eq. 5) then
              call test_mpi3rma_dynamic_ddt_1( base, 3*DIM1, win, comm )
            else
              write (*,*) "Memory region is too small for the test. ", &
                "DIM1 has to be at least 5."
              STOP
            endif

!=============================================================================
!==================== dynamic MPI Datatype test 4 ============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - Accumulate/SUM with DDTs (in more than one memory ", &
                "region) (fence)"
              test_counter = test_counter + 1
            endif

            if (DIM1 .eq. 5) then
              call test_mpi3rma_dynamic_ddt_4( base, 3*DIM1, win, comm )
            else
              write (*,*) "DIM1 has to be 5 to run this test."
              STOP
            endif

!           revert back to the original state
            win_size = DIM1*4 ! assuming integer size of 4
            call foMPI_Win_attach( win, base(DIM1+1), win_size, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 707, grank)

            call MPI_Barrier( comm, mpierr )

!=============================================================================
!==================== dynamic MPI Datatype test 2 ============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - Put with DDTs (in more than one memory ", &
                "region/contiguous blocks aren't aligned with memory ", &
                "regions boundaries) (fence)"
              test_counter = test_counter + 1
            endif

            if (DIM1 .eq. 5) then
              call test_mpi3rma_dynamic_ddt_2( base, 3*DIM1, win, comm )
            else
              write (*,*) "DIM1 has to be 5 to run this test."
              STOP
            endif

!=============================================================================
!==================== dynamic MPI Datatype test 3 ============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - Put (contiguous over more than one memory regions) ", &
                "(fence)"
              test_counter = test_counter + 1
            endif

!            only useful, if there is more than one memory region */
            call test_mpi3rma_dynamic_ddt_3( base, 3*DIM1, DIM1, win, comm )
 
!=============================================================================
!==================== dynamic MPI Datatype test 5 ============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - Accumulate/SUM with DDTs (in more than one memory ", &
                "region/contiguous blocks aren't aligned with memory ", &
                "regions boundaries) (fence)"
              test_counter = test_counter + 1
            endif

            if (DIM1 .eq. 5) then
              call test_mpi3rma_dynamic_ddt_5( base, 3*DIM1, win, comm )
            else
              write (*,*) "DIM1 has to be 5 to run this test."
              STOP
            endif

!=============================================================================
!==================== dynamic MPI Datatype test 6 ============================
!=============================================================================

            if (rank .eq. 0) then
              write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
                " - Accumulate/SUM (contiguous over more than one ", &
                "memory regions) (fence)"
              test_counter = test_counter + 1
            endif

!           only useful, if there is more than one memory region
            call test_mpi3rma_dynamic_ddt_6( base, 3*DIM1, DIM1, win, comm )
          endif

          if ( j .lt. 2 ) then
            call foMPI_Win_free( win, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 780, grank)
          endif

          call foMPI_Win_free( win2, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 784, grank)

          if (j .eq. 0 ) then
            deallocate( base, stat=mpierr )
          endif

!=============================================================================
!==================== get_accumulate maxloc lock test ========================
!=============================================================================

!         for this kind of test we need a different kind of window
          select case (j)
            case (0)
              allocate( base3(2,DIM1), stat=mpierr )
              ! assuming that type size of real is 4
              win_size = DIM1 * 2 * 4
              call foMPI_Win_create( base3(1,1), win_size, 8, MPI_INFO_NULL, comm, win, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 801, grank)
            case (1)
              ! assuming that type size of real is 4
              win_size = DIM1 * 2 * 4
              call foMPI_Win_allocate( win_size, 8, MPI_INFO_NULL, comm, ptr, win, mpierr );
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 806, grank)
              shape_2d = (/2,DIM1/)
              call c_f_pointer( ptr, base3, shape_2d )
            case (2)
              allocate( base3(2,DIM1), stat=mpierr )
              win_size = DIM1 * 2 * 4 ! assuming that type size of real is 4
              call foMPI_Win_attach( win, base3(1,1), win_size, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 813, grank)

              call MPI_Barrier( comm, mpierr )
          end select

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Get_accumulate/MAXLOC (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_get_acc_maxloc_lock(base3, DIM1, DIM1, win, comm)

          if ( j .lt. 2 ) then
            call foMPI_Win_free( win, mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 827, grank)
          else
            call foMPI_Win_detach( win, base3(1,1), mpierr )
            call check_fompi_status(mpierr, MPI_SUCCESS, filename, 829, grank)
          endif

          if (j .eq. 0 ) then
            deallocate( base3, stat=mpierr )
          endif

!=============================================================================
!================== compare_and_swap/special fence test ======================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

!         for this kind of test we need a different kind of window
          select case (j)
            case (0)
              allocate( base4(1), stat=mpierr)
              win_size = 8
              call foMPI_Win_create( base4(1), win_size, 8, MPI_INFO_NULL, comm, win, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 849, grank)
            case (1)
              win_size = 8
              call foMPI_Win_allocate( win_size, 8, MPI_INFO_NULL, comm, ptr, win, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 853, grank)
              shape_1d = 1
              call c_f_pointer( ptr, base4, shape_1d )
            case (2)
              allocate( base4(1), stat=mpierr )
              win_size = 8
              call foMPI_Win_attach( win, base4(1), win_size, mpierr )
              call check_fompi_status(mpierr, MPI_SUCCESS, filename, 860, grank)

              call MPI_Barrier( comm, mpierr )
          end select

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Compare_and_swap/special (fence)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_cswap_special_fence(base4, 1, win, comm)

!=============================================================================
!================== compare_and_swap/special pscw test =======================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then 
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Compare_and_swap/special (PSCW)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_cswap_special_pscw(base4, 1, win, comm)

!=============================================================================
!================== compare_and_swap/special lock test =======================
!=============================================================================

          call MPI_Barrier( comm, mpierr ) ! synchronisation mode change

          if (rank .eq. 0) then
            write (*,*) "test ", test_counter, ": ", trim(create_flavor), &
              " - Compare_and_swap/special (lock)"
            test_counter = test_counter + 1
          endif
          call test_mpi3rma_cswap_special_lock(base4, 1, win, comm)

          call foMPI_Win_free( win, mpierr )
          call check_fompi_status(mpierr, MPI_SUCCESS, filename, 899, grank)

          if (j .ne. 1 ) then
            deallocate( base4, stat=mpierr )
            if (j .eq. 2) then
              deallocate( base3, stat=mpierr )
              deallocate( base, stat=mpierr )
            endif
          endif

          time(j+1) = time(j+1) + MPI_Wtime()
        enddo

        if( grank .eq. 0 ) then
          do j=1,3
            write (*,*) j, time(j)*1000000
          enddo
        endif

        if (i .ne. 0) then
          call MPI_Comm_free( comm, mpierr )
        endif

      enddo

      call foMPI_Finalize( mpierr )

      end program test_mpi3rma_fortran
