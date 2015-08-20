MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
@EFL_CFLAGS@ \
-I$(top_srcdir)/src/lib

include $(top_srcdir)/Makefile_Estate_Helper.am

BUILT_SOURCES = turnstile.est.c

EXTRA_DIST = turnstile.est

bin_PROGRAMS = turnstile
turnstile_SOURCES = turnstile.c
turnstile_LDADD = \
@EFL_LIBS@ \
$(top_builddir)/src/lib/libestate.la

turnstile.est.c: turnstile.est Makefile
	$(AM_V_EST)$(top_builddir)/src/bin/estate_cc --gc --output $@ $<

CLEANFILES = $(BUILT_SOURCES)

