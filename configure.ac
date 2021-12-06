AC_INIT(get_config, 1.2)
AC_CONFIG_SRCDIR(src/main.c)
AM_INIT_AUTOMAKE

dnl keep the correct libtool macros in-tree
AC_CONFIG_MACRO_DIR([m4])

dnl use config.h for defines
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_ISC_POSIX

dnl Checks for libraries.	

dnl Checks for header files.
AC_STDC_HEADERS
AC_HAVE_HEADERS(string.h fcntl.h sys/file.h sys/param.h)

dnl optional headers/libraries

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([getline])


dnl Check for st_blksize in struct stat
AC_ST_BLKSIZE

dnl Default compiler flags
CPPFLAGS="-pedantic -Wall -Wextra" 

AC_OUTPUT([
 Makefile \
 src/Makefile \
])

dnl *****************
dnl Configure summary
dnl *****************

echo
echo "$PACKAGE_NAME configuration:"
echo
echo "  Version:		$PACKAGE_VERSION"
echo "  Install prefix:	$prefix"
echo 
echo
