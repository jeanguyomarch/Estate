MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
@EFL_CFLAGS@ \
-I$(top_srcdir)/src/lib

include $(top_srcdir)/Makefile_Estate_Helper.am

bin_PROGRAMS = turnstile hello

turnstile_SOURCES = turnstile.c
turnstile_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

turnstile.est.c: turnstile.est Makefile
	$(AM_V_EST)$(top_builddir)/src/bin/estate_cc --gc --output $@ $<


hello_SOURCES = hello.c
hello_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

hello.est.c: hello.est Makefile
	$(AM_V_EST)$(top_builddir)/src/bin/estate_cc --gc --output $@ $<


BUILT_SOURCES = turnstile.est.c hello.est.c
EXTRA_DIST = turnstile.est hello.est
CLEANFILES = $(BUILT_SOURCES)

