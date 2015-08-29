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
estate_cc_out_gc.c  \
estate_cc_out_gi.c  \
estate_cc_out_pgf_tikz.c \
estate_cc_check.c   \
estate_cc_misc.c    \
estate_cc.h

estate_cc_LDADD = @EFL_LIBS@

