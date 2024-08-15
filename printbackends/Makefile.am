include $(top_srcdir)/Makefile.decl

SUBDIRS = 

if HAVE_PRINT
SUBDIRS += file lpr
endif

if HAVE_CUPS
SUBDIRS += cups
endif

if HAVE_PAPI
SUBDIRS += papi
endif

if BUILD_TESTS
SUBDIRS += test
endif


DIST_SUBDIRS = cups file lpr papi test

-include $(top_srcdir)/git.mk
