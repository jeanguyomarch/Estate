if HAVE_TESTS

check_PROGRAMS = estate_suite
TESTS = estate_suite

estate_suite_SOURCES = \
estate_suite.c \
estate_suite.h \
estate_main.c

estate_suite_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
@CHECK_CFLAGS@ \
@EFL_CFLAGS@

estate_suite_LDADD = \
@EFL_LIBS@ \
@CHECK_LIBS@ \
$(top_builddir)/src/lib/libestate.la

endif HAVE_TESTS

