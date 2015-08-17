MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EFL_CFLAGS@

bin_PROGRAMS = estate_cc

estate_cc_SOURCES = \
estate_cc_parser.c  \
estate_cc_log.c     \
estate_cc_main.c    \
estate_cc_data.c    \
estate_cc_out.c     \
estate_cc.h

estate_cc_LDADD = @EFL_LIBS@

