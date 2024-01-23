include $(top_srcdir)/Makefile.decl

SUBDIRS = file lpr

if HAVE_CUPS
SUBDIRS += cups
endif

if HAVE_PAPI
SUBDIRS += papi
endif

DIST_SUBDIRS = cups file lpr papi

-include $(top_srcdir)/git.mk
