# preset some vars so += can be used.

bin_PROGRAMS =
noinst_LIBRARIES =
noinst_PROGRAMS =
lib_LIBRARIES =
noinst_HEADERS =
TESTS =
check_PROGRAMS = 

AM_CPPFLAGS = -I${top_srcdir}/src/dataloop -I${top_srcdir}/src/mpitypes


#
# pkgconfig files
#
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpitypes.pc
$(pkgconfig_DATA): config.status

# include files defining source files
include $(top_srcdir)/src/Makefile.subdir
include $(top_srcdir)/test/Makefile.subdir

