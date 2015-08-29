MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
@EFL_CFLAGS@ \
-I$(top_srcdir)/src/lib

include $(top_srcdir)/Makefile_Estate_Helper.am

bin_PROGRAMS = bench
bench_SOURCES = bench.c
bench_LDADD = \
@EFL_LIBS@ \
$(top_builddir)/src/lib/libestate.la

bench.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/bench.est

BUILT_SOURCES = bench.est.c
EXTRA_DIST = bench.est

clean-local:
	$(RM) *.est.c

