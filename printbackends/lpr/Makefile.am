include $(top_srcdir)/Makefile.decl


INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/gtk				\
	-I$(top_builddir)/gtk				\
	-I$(top_srcdir)/gdk				\
	-I$(top_builddir)/gdk				\
	-DGTK_PRINT_BACKEND_ENABLE_UNSUPPORTED		\
	$(GTK_DEP_CFLAGS)				\
	$(GTK_DEBUG_FLAGS)

LDADDS = \
	$(GTK_DEP_LIBS)					\
	$(top_builddir)/gtk/$(gtktargetlib)		

backenddir = $(libdir)/gtk-2.0/$(GTK_BINARY_VERSION)/printbackends

backend_LTLIBRARIES = libprintbackend-lpr.la

libprintbackend_lpr_la_SOURCES =	\
	gtkprintbackendlpr.c

noinst_HEADERS =			\
	gtkprintbackendlpr.h

libprintbackend_lpr_la_LDFLAGS =  -avoid-version -module $(no_undefined)
libprintbackend_lpr_la_LIBADD = $(LDADDS)

-include $(top_srcdir)/git.mk
