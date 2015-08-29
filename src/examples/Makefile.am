MAINTAINERCLEANFILES = Makefile.in
AUTOMAKE_OPTIONS = 1.4 foreign

AM_CPPFLAGS = \
@EFL_CFLAGS@ \
-I$(top_srcdir)/src/lib

include $(top_srcdir)/Makefile_Estate_Helper.am

bin_PROGRAMS = turnstile hello estate_parser reentrant

turnstile_SOURCES = turnstile.c
turnstile_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

turnstile.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/turnstile.est


hello_SOURCES = hello.c
hello_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

hello.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/hello.est


estate_parser_SOURCES = estate_parser.c
estate_parser_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

estate_parser.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/estate_parser.est


reentrant_SOURCES = reentrant.c
reentrant_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libestate.la

reentrant.est.c: Makefile $(ESTATE_CC)
	$(AM_V_EST)$(ESTATE_CC) --gc --output $@ $(srcdir)/reentrant.est



BUILT_SOURCES = turnstile.est.c hello.est.c estate_parser.est.c reentrant.est.c
EXTRA_DIST = turnstile.est hello.est estate_parser.est reentrant.est

clean-local:
	$(RM) *.est.c

