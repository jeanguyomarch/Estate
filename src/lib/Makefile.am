MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
@EFL_CFLAGS@

lib_LTLIBRARIES = libestate.la

includes_HEADERS = \
Estate.h

includesdir = $(includedir)/estate-@VMAJ@

libestate_la_SOURCES = \
estate_main.c \
estate_private.h

libestate_la_LIBADD = @EFL_LIBS@
libestate_la_LDFLAGS = -no-undefined @EFL_LTLIBRARY_FLAGS@

