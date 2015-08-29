MAINTAINERCLEANFILES = Makefile.in
AUTOMAKE_OPTIONS = 1.4 foreign

AM_CPPFLAGS = \
@EFL_CFLAGS@ \
-I$(top_srcdir)/src/lib

include $(top_srcdir)/Makefile_Estate_Helper.am

bin_PROGRAMS = turnstile hello

turnstile_SOURCES = turnstile.c
turnstile_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

turnstile.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/turnstile.est

hello_SOURCES = hello.c
hello_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

hello.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/hello.est

BUILT_SOURCES = turnstile.est.c hello.est.c
EXTRA_DIST = turnstile.est hello.est

clean-local:
	$(RM) *.est.c

