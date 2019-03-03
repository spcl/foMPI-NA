dnl MPITypes AUTOCONF SCRIPT

dnl
dnl major: inc if interfaces added,changed or removed
dnl minor: inc if source has changed
dnl sub:  inc if interfaces added; set to zero if removed/changed
dnl
m4_define([M4_MPITYPES_VERSION_MAJOR],[0])
m4_define([M4_MPITYPES_VERSION_MINOR],[0])
m4_define([M4_MPITYPES_VERSION_SUB],[3])
m4_define([M4_MPITYPES_VERSION],M4_MPITYPES_VERSION_MAJOR.M4_MPITYPES_VERSION_MINOR.M4_MPITYPES_VERSION_SUB)

dnl
dnl release is own choice
dnl
m4_define([M4_MPITYPES_RELEASE],[1])

AC_INIT([mpitypes],[M4_MPITYPES_VERSION])

AC_PREREQ(2.59)

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src/mpitypes/mpitypes.c])

MPITYPES_VERSION=M4_MPITYPES_VERSION
MPITYPES_RELEASE=M4_MPITYPES_RELEASE
AC_SUBST(MPITYPES_VERSION)
AC_SUBST(MPITYPES_RELEASE)

dnl
dnl Check that we've got an cc we can use.
dnl
if test "x$CC" != "x"; then
   AC_MSG_WARN([Ignoring CC value ($CC); please use --with-mpi to specify
                alternative MPI location instead.])
fi

AC_ARG_WITH(mpi,
	[ --with-mpi=<dir>	Build with specified MPI.],
	[USR_MPI=${withval}],
	[USR_MPI=""])

if test "x$USR_MPI" != "x"; then
   USR_PATH=$PATH
   PATH=$USR_MPI/bin:$USR_PATH
   BUILD_CC=$USR_MPI/bin/cc
else
   BUILD_CC=cc
fi

CC=cc
AC_PROG_CC
AM_PROG_CC_C_O

AC_PROG_RANLIB

dnl
dnl Gain confidence that the cc works.
dnl
AC_CHECK_HEADER(mpi.h,,AC_MSG_ERROR([Cannot include mpi.h; something is broken with your MPI compiler.]))
AC_CHECK_FUNC(MPI_Init,,AC_MSG_ERROR([Cannot find function MPI_Init; something is broken with your MPI compiler.]))

# determine sizes of key types
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long int])
AC_CHECK_SIZEOF([long long int])
AC_CHECK_SIZEOF([MPI_Aint],[],[
    AC_INCLUDES_DEFAULT
    #include "mpi.h"])

AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T

# DLOOP_Offset printf format specifiers
aint_dec_spec=""
aint_hex_spec=""
if test "$ac_cv_sizeof_MPI_Aint" = "$ac_cv_sizeof_int" ; then
    aint_dec_spec="%d"
    aint_hex_spec="%x"
elif test "$ac_cv_sizeof_MPI_Aint" = "$ac_cv_sizeof_long_int" ; then
    aint_dec_spec="%ld"
    aint_hex_spec="%lx"
elif test "$ac_cv_sizeof_MPI_Aint" = "$ac_cv_sizeof_long_long_int" ; then
    aint_dec_spec="%lld"
    aint_hex_spec="%llx"
else
    AC_MSG_ERROR([unable to determine a printf specification for MPI_Aint])
fi
AC_DEFINE_UNQUOTED([DLOOP_OFFSET_FMT_DEC_SPEC],["$aint_dec_spec"],
                   [a printf decimal format specifier for DLOOP_Offset])
AC_DEFINE_UNQUOTED([DLOOP_OFFSET_FMT_HEX_SPEC],["$aint_hex_spec"],
                   [a printf hexadecimal format specifier for DLOOP_Offset])

dnl Verify that the MPI-2 functions we use are present.
AC_CHECK_FUNC(MPI_Type_get_envelope,,AC_MSG_ERROR([Cannot find required function.]))
AC_CHECK_FUNC(MPI_Type_get_contents,,AC_MSG_ERROR([Cannot find required function.]))
AC_CHECK_FUNC(MPI_Type_get_true_extent,,AC_MSG_ERROR([Cannot find required function.]))
AC_CHECK_FUNC(MPI_Type_get_attr,,AC_MSG_ERROR([Cannot find required function.]))
AC_CHECK_FUNC(MPI_Type_set_attr,,AC_MSG_ERROR([Cannot find required function.]))
AC_CHECK_FUNC(MPI_Comm_create_keyval,,AC_MSG_ERROR([Cannot find required function.]))

AC_SUBST(BUILD_CC)

dnl
dnl Look for internal MPICH2 contents access optimization function (optional).
dnl
AC_MSG_CHECKING([for MPICH2 internal contents access function])
AC_TRY_COMPILE(
   [
      #include <mpi.h>
   ],
   [
      int *intp;
      MPI_Aint *aintp;
      MPI_Datatype *typep;

      MPID_Type_access_contents(MPI_DATATYPE_NULL, &intp, &aintp, &typep);
   ],
   AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_MPID_TYPE_ACCESS_CONTENTS, 1, [Define if MPICH2 internal
             contents access function is available])
   ,
   AC_MSG_RESULT(no)
)


AC_CHECK_HEADER(string.h)

# check for __attribute__ specifier
# adapted from PAC_C_GNU_ATTRIBUTE
# we don't strictly require gcc
AC_CACHE_CHECK([whether __attribute__ allowed],
pac_cv_gnu_attr_pure,[
AC_TRY_COMPILE([int foo(int) __attribute__ ((pure));],[int a;],
pac_cv_gnu_attr_pure=yes,pac_cv_gnu_attr_pure=no)])
AC_CACHE_CHECK([whether __attribute__((format)) allowed],
pac_cv_gnu_attr_format,[
AC_TRY_COMPILE([int foo(char *,...) __attribute__ ((format(printf,1,2)));],[int a;],
pac_cv_gnu_attr_format=yes,pac_cv_gnu_attr_format=no)])
if test "$pac_cv_gnu_attr_pure" = "yes" -a "$pac_cv_gnu_attr_format" = "yes" ; then
    AC_DEFINE(HAVE_GCC_ATTRIBUTE,[1],[Define if GNU __attribute__ is supported])
fi


AC_CONFIG_HEADER(mpitypes-config.h)
AC_CONFIG_FILES([Makefile mpitypes.pc])

AC_OUTPUT

