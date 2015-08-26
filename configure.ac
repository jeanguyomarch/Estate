EFL_VERSION([1], [0], [0], [dev])
AC_INIT([estate], [efl_version], [jean.guyomarch@gmail.com])

AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef __ESTATE_H__
#define __ESTATE_H__
])
AH_BOTTOM([
#endif /* ! __ESTATE_H__ */
])

AM_INIT_AUTOMAKE([1.6 dist-xz -Wall color-tests foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_PREREQ([2.2])
LT_INIT([win32-dll pic-only])
EFL_INIT

AC_PROG_CC

PKG_PROG_PKG_CONFIG

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

efl_lib_version="1.7"
PKG_CHECK_MODULES([EFL], [
   eina       >= ${efl_lib_version}
])

AC_ARG_ENABLE([tests],
   [AS_HELP_STRING([--disable-tests],[Disable test suite. @<:@default=enabled@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_tests="yes"
    else
       want_tests="no"
    fi
   ],
   [want_tests="yes"])

if test "x${want_tests}" = "xyes" ; then
   PKG_CHECK_MODULES([CHECK], [check >= 0.9.5])
fi
AM_CONDITIONAL([HAVE_TESTS], [test "x${want_tests}" = "xyes"])

AM_PROG_CC_C_O

EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-Wextra])
EFL_COMPILER_FLAG([-Wshadow])
EFL_COMPILER_FLAG([-Wmissing-declarations])
EFL_COMPILER_FLAG([-Wfloat-equal])
EFL_COMPILER_FLAG([-Wpointer-arith])
EFL_COMPILER_FLAG([-Wno-missing-field-initializers])
EFL_COMPILER_FLAG([-Winline])
EFL_COMPILER_FLAG([-Winit-self])


AC_CONFIG_FILES([
   Makefile
   pc/estate.pc
   doc/Makefile
   doc/Doxyfile
   src/Makefile
   src/lib/Makefile
   src/bin/Makefile
   src/tests/Makefile
   src/examples/Makefile
   src/benchmarks/Makefile
])
AC_OUTPUT

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Compilation..............: make (or gmake)"
echo "  CPPFLAGS...............: $CPPFLAGS"
echo "  CFLAGS.................: $CFLAGS"
echo "  LDFLAGS................: $LDFLAGS"
echo
echo "Building documentation...: ${build_doc}"
echo "Building test suite......: ${want_tests}"
echo
echo "Installation.............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix.................: $prefix"
echo

