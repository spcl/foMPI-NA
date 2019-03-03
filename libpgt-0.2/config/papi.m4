AC_DEFUN([PGT_WITH_PAPI],
    [AC_ARG_WITH(papi, AC_HELP_STRING([--with-papi], [compile with PAPI support (ARG can be the path to the location where PAPI was installed)]))
    papi_found=no
	AC_SUBST([PAPI_CFLAGS], [])
	AC_SUBST([PAPI_LDFLAGS], [])
    if test x"${with_papi}" = xyes; then
        AC_CHECK_HEADER(papi.h, papi_found=yes, [AC_MSG_ERROR([PAPI support selected but headers not available!])])
    elif test x"${with_papi}" = xno; then
		papi_found=no
	elif test x"${with_papi}" != x; then
        AC_CHECK_HEADER(${with_papi}/include/papi.h, [pgt_papi_path=${with_papi}; papi_found=yes], [AC_MSG_ERROR([Can't find the PAPI header files in ${with_papi}/include/])])
    else
        AC_CHECK_HEADER(papi.h, papi_found=yes, [AC_MSG_NOTICE([PAPI support disabled])])
    fi
    if test x"${papi_found}" = xyes; then
        AC_DEFINE(HAVE_PAPI, 1, enables PAPI)
        AC_MSG_NOTICE([PAPI support enabled])
		AC_SUBST([PAPI_CFLAGS], [])
		AC_SUBST([PAPI_LDFLAGS], [-lpapi])
        if test x${pgt_papi_path} != x; then
            CFLAGS="${CFLAGS} -I${pgt_papi_path}/include"
            CXXFLAGS="${CXXFLAGS} -I${pgt_papi_path}/include"
            LDFLAGS="${LDFLAGS} -lpapi -L${pgt_papi_path}/lib -L${pgt_papi_path}/lib64"
			AC_SUBST([PAPI_CFLAGS], [-I${pgt_papi_path}/include])
			AC_SUBST([PAPI_LDFLAGS], [-lpapi -L${pgt_papi_path}/lib -L${pgt_papi_path}/lib64])
        fi
    fi
    ]
)

