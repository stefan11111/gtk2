include $(top_srcdir)/Makefile.decl

backenddir = $(libdir)/gtk-2.0/$(GTK_BINARY_VERSION)/printbackends

backend_LTLIBRARIES = libprintbackend-test.la

libprintbackend_test_la_SOURCES = \
	gtkprintbackendtest.c

libprintbackend_test_la_CPPFLAGS = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/gtk				\
	-I$(top_builddir)/gtk				\
	-I$(top_srcdir)/gdk				\
	-I$(top_builddir)/gdk				\
	-DGTK_PRINT_BACKEND_ENABLE_UNSUPPORTED		\
	$(AM_CPPFLAGS)

libprintbackend_test_la_CFLAGS = \
	$(GTK_DEP_CFLAGS)				\
	$(AM_CFLAGS)

libprintbackend_test_la_LDFLAGS =  \
	-avoid-version -module $(no_undefined)

libprintbackend_test_la_LIBADD = \
	$(top_builddir)/gtk/$(gtktargetlib)		\
	$(GTK_DEP_LIBS)

noinst_HEADERS =			\
	gtkprintbackendtest.h

-include $(top_srcdir)/git.mk
