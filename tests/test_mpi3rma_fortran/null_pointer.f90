      ! currently doesn't work with gcc

      subroutine test_mpi3rma_null_pointer ( comm )

#if INCLUDE_TYPE==1
      implicit none
      
      include 'fompif.h'
#endif
#if INCLUDE_TYPE==2
      use fompi
      
      implicit none
#endif

      integer, intent(in) :: comm

      integer :: res
      integer :: res_free
      character(20) :: create_flavor
      integer :: win
      integer :: j
      integer(kind=MPI_ADDRESS_KIND) :: win_size

      do j=0,1
        ! test if a NULL pointer as base is valid */
        if (j .eq. 0) then
          win_size = 0
          call foMPI_Win_create(null(), win_size, 4, MPI_INFO_NULL, comm, win, res);
          write(create_flavor,*) "create"
        else
          win_size = 0
          call foMPI_Win_allocate( win_size, 4, MPI_INFO_NULL, comm, null(), win, res)
          write(create_flavor,*) "allocate"
        endif
        call foMPI_Win_free( win, res_free )

        if ( (res .eq. MPI_SUCCESS) .and. (res_free .eq. MPI_SUCCESS) ) then
          write (*,*) trim(create_flavor), " - No Error"
        else
          write (*,*) "Error: Return code of MPI_Win_", trim(create_flavor), &
            " was ", res, " and of MPI_Win_free was ", res_free, &
            " instead of ", MPI_SUCCESS
        endif
      enddo

      end subroutine test_mpi3rma_null_pointer
