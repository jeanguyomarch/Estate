ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc

MAINTAINERCLEANFILES =	\
Makefile.in		\
aclocal.m4		\
ar-lib			\
compile			\
config.guess		\
config.h.in		\
config.sub		\
configure		\
depcomp			\
install-sh		\
ltmain.sh		\
m4/libtool.m4		\
m4/ltoptions.m4		\
m4/ltsugar.m4		\
m4/ltversion.m4		\
m4/lt~obsolete.m4	\
missing

EXTRA_DIST =			\
m4/efl.m4			\
m4/efl_binary.m4		\
m4/efl_compiler_flag.m4		\
m4/efl_doxygen.m4		\
README.md			\
AUTHORS

.PHONY: doc

doc:
	$(MAKE) $(AM_MAKEFLAGS) -C doc doc

