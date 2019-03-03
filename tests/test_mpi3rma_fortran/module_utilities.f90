      module utilities

      contains

      subroutine check_fompi_status( actual, expected, filename, line, &
        rank )

      implicit none

      integer, intent(in) :: actual
      integer, intent(in) :: expected
      character(50), intent(in) :: filename
      integer, intent(in) :: line
      integer, intent(in) :: rank

      if (actual .ne. expected) then
        write (*,*) "Error on rank ", rank, " in ", filename, " line ", &
          line, ": return code ", actual, " instead of ", expected
        STOP
      endif

      end subroutine check_fompi_status

      subroutine millisleep(milliseconds)
      ! inspired by IRO-bot (see http://stackoverflow.com/questions/6931846/sleep-in-fortran)
      ! for small waits, since it is using CPU time while waiting

      implicit none

      integer, intent(in) :: milliseconds
      integer, dimension(8) :: t
      integer :: ms1, ms2

      ! Get start time:
      call date_and_time( values=t )
      ms1 = ( t(5) * 3600 + t(6) *60 + t(7) ) * 1000+ t(8)

      do ! check time:
        call date_and_time(values=t)
        ms2 = ( t(5) * 3600 + t(6) * 60 + t(7) ) * 1000 + t(8)
        if ( ms2 - ms1 >= milliseconds ) exit
      enddo
      
      end subroutine millisleep

      end module utilities
