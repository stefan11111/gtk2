## Makefile.am for gtk+/gdk
include $(top_srcdir)/Makefile.decl
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = \
   --warn-all \
   --add-include-path=../gdk
INTROSPECTION_COMPILER_ARGS = \
   --includedir=$(srcdir) \
   --includedir=.

SUBDIRS = $(gdktarget) .
DIST_SUBDIRS = x11 directfb

if BUILD_TESTS
SUBDIRS += tests
DIST_SUBDIRS += tests
endif

CLEANFILES =

EXTRA_DIST +=			\
	keynames.txt		\
	keyname-table.h		\
	gen-keyname-table.pl	\
	gdkkeysyms-update.pl	\
	gdkmedialib.h		\
	gdkwindowimpl.h		\
	makeenums.pl		\
	gdkenumtypes.c.template	\
	gdkenumtypes.h.template

INCLUDES =                              \
	-DG_LOG_DOMAIN=\"Gdk\"		\
	-DGDK_COMPILATION		\
	-I$(top_srcdir)			\
	-I$(top_builddir)		\
	-I$(top_builddir)/gdk		\
	$(GTK_DEBUG_FLAGS) 		\
	$(GDK_DEP_CFLAGS)

gtarget=$(gdktarget)

# libtool stuff: set version and export symbols for resolving
# since automake doesn't support conditionalized libsomething_la_LDFLAGS
# we use the general approach here
LDADD = \
	$(GTK_LINK_FLAGS)						\
	-version-info $(LT_VERSION_INFO)		 		\
	-export-dynamic 						\
	-rpath $(libdir) 						\
	$(no_undefined)							\
	$(LIBTOOL_EXPORT_OPTIONS)

if USE_MEDIALIB
medialib_sources =  \
    gdkmedialib.c
else
medialib_sources =
endif

#
# setup source file variables
#
#
# GDK header files for public installation (non-generated)
#
gdk_public_h_sources =				\
	gdk.h					\
	gdkapplaunchcontext.h			\
	gdkcairo.h				\
	gdkcolor.h				\
	gdkcursor.h				\
	gdkdisplay.h				\
	gdkdisplaymanager.h			\
	gdkdnd.h				\
	gdkdrawable.h				\
	gdkevents.h				\
	gdkfont.h				\
	gdkgc.h					\
	gdkimage.h				\
	gdkinput.h				\
	gdkkeys.h				\
	gdkkeysyms.h				\
	gdkkeysyms-compat.h			\
	gdkpango.h				\
	gdkpixbuf.h				\
	gdkpixmap.h				\
	gdkprivate.h				\
	gdkproperty.h				\
	gdkregion.h				\
	gdkrgb.h				\
	gdkscreen.h				\
	gdkselection.h				\
	gdkspawn.h				\
	gdktestutils.h				\
	gdktypes.h				\
	gdkvisual.h				\
	gdkwindow.h

gdk_built_public_sources =			\
	gdkenumtypes.h

gdk_built_private_headers =

gdk_private_headers =   \
	gdkinternals.h \
	gdkintl.h \
    gdkpoly-generic.h	\
	gdkregion-generic.h

gdk_c_sources =                 \
	$(medialib_sources)     \
	gdk.c			\
	gdkapplaunchcontext.c	\
	gdkcairo.c		\
	gdkcolor.c		\
	gdkcursor.c		\
	gdkdisplay.c		\
	gdkdisplaymanager.c	\
	gdkdnd.c		\
	gdkdraw.c		\
	gdkevents.c     	\
	gdkfont.c		\
	gdkgc.c			\
	gdkglobals.c		\
	gdkimage.c		\
	gdkkeys.c		\
	gdkkeyuni.c		\
	gdkoffscreenwindow.c	\
	gdkpango.c		\
	gdkpixbuf-drawable.c	\
	gdkpixbuf-render.c	\
	gdkpixmap.c		\
	gdkpolyreg-generic.c	\
	gdkrectangle.c		\
	gdkregion-generic.c	\
	gdkrgb.c		\
	gdkscreen.c		\
	gdkselection.c		\
	gdkvisual.c		\
	gdkwindow.c		\
	gdkwindowimpl.c

gdk_built_sources =				\
	gdkenumtypes.c				\
	$(gdk_built_public_sources)		\
	$(gdk_built_private_headers)

#
# setup GDK sources and their dependencies
#

gdkincludedir = $(includedir)/gtk-2.0/gdk
gdkinclude_HEADERS = $(gdk_public_h_sources) $(gdk_built_public_sources)

common_sources =                \
	$(gdk_private_headers)      \
    $(gdk_c_sources)	\
	gdkenumtypes.c

libgdk_directfb_2_0_la_SOURCES = $(common_sources)
libgdk_directfb_2_0_la_LIBADD = directfb/libgdk-directfb.la $(GDK_DEP_LIBS)
libgdk_directfb_2_0_la_LDFLAGS = $(LDADD)

libgdk_x11_2_0_la_SOURCES = $(common_sources)
libgdk_x11_2_0_la_LIBADD = x11/libgdk-x11.la $(GDK_DEP_LIBS)
libgdk_x11_2_0_la_LDFLAGS = $(LDADD)

if HAVE_INTROSPECTION

base_introspection_files = 		\
	$(filter-out gdkkeysyms-compat.h, $(gdk_public_h_sources))	\
	$(gdk_c_sources)	\
	gdkkeynames.c		\
        gdkenumtypes.c		\
        gdkenumtypes.h

introspection_files = $(base_introspection_files)

Gdk-2.0.gir: $(gdktargetlib) Makefile
Gdk_2_0_gir_SCANNERFLAGS = --c-include="gdk/gdk.h"
Gdk_2_0_gir_INCLUDES = Gio-2.0 GdkPixbuf-2.0 Pango-1.0 cairo-1.0
Gdk_2_0_gir_LIBS = $(gdktargetlib)
Gdk_2_0_gir_FILES = $(introspection_files)
Gdk_2_0_gir_EXPORT_PACKAGES = gdk-2.0
Gdk_2_0_gir_CFLAGS = $(INCLUDES)
INTROSPECTION_GIRS += Gdk-2.0.gir

if USE_X11
x11_introspection_files = 		\
	x11/checksettings.c		\
	x11/gdkapplaunchcontext-x11.c	\
	x11/gdkasync.c			\
	x11/gdkcolor-x11.c		\
	x11/gdkcursor-x11.c		\
	x11/gdkdisplay-x11.c		\
	x11/gdkdnd-x11.c		\
	x11/gdkdrawable-x11.c		\
	x11/gdkgc-x11.c			\
	x11/gdkgeometry-x11.c		\
	x11/gdkglobals-x11.c		\
	x11/gdkimage-x11.c		\
	x11/gdkim-x11.c			\
	x11/gdkinput.c			\
	x11/gdkkeys-x11.c		\
	x11/gdkmain-x11.c		\
	x11/gdkpixmap-x11.c		\
	x11/gdkproperty-x11.c		\
	x11/gdkscreen-x11.c		\
	x11/gdkselection-x11.c		\
	x11/gdksettings.c		\
	x11/gdkspawn-x11.c		\
	x11/gdktestutils-x11.c		\
	x11/gdkvisual-x11.c		\
	x11/gdkwindow-x11.c		\
	x11/gdkxftdefaults.c		\
	x11/gdkxid.c			\
	x11/xsettings-client.c		\
	x11/xsettings-common.c		\
        x11/gdkx.h

GdkX11-2.0.gir: $(gdktargetlib) Gdk-2.0.gir Makefile
GdkX11_2_0_gir_SCANNERFLAGS = --identifier-prefix=Gdk --include-uninstalled=Gdk-2.0.gir
GdkX11_2_0_gir_INCLUDES = Gio-2.0 GdkPixbuf-2.0 Pango-1.0 xlib-2.0

GdkX11_2_0_gir_LIBS = $(gdktargetlib)
GdkX11_2_0_gir_FILES = $(x11_introspection_files)
GdkX11_2_0_gir_EXPORT_PACKAGES = gdk-x11-2.0
GdkX11_2_0_gir_CFLAGS = $(INCLUDES) -L$(top_builddir)/gdk
INTROSPECTION_GIRS += GdkX11-2.0.gir

introspection_files += $(filter-out x11/gdkx.h, $(x11_introspection_files))

endif # USE_X11

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)

endif # HAVE_INTROSPECTION


lib_LTLIBRARIES = $(gdktargetlib)

EXTRA_LTLIBRARIES = libgdk-x11-2.0.la libgdk-directfb-2.0.la

MAINTAINERCLEANFILES = $(gdk_built_sources) stamp-gdkenumtypes.h
EXTRA_DIST += $(gdk_built_sources)
EXTRA_HEADERS =

#
# Rule to install gdkconfig.h header file
#
configexecincludedir = $(libdir)/gtk-2.0/include
#configexecinclude_DATA = gdkconfig.h

install-exec-local: gdkconfig.h
	$(mkinstalldirs) $(DESTDIR)$(configexecincludedir)
	file=$(DESTDIR)$(configexecincludedir)/gdkconfig.h; \
	if test -r $$file && cmp -s gdkconfig.h $$file; then :; \
	else $(INSTALL_DATA) gdkconfig.h $$file; fi

install-exec-hook:
if DISABLE_EXPLICIT_DEPS
	$(SHELL) $(top_srcdir)/sanitize-la.sh $(DESTDIR)$(libdir)/$(gdktargetlib)
endif

#note: not gdkconfig.h
BUILT_SOURCES = \
	$(gdk_built_sources)			\
	gdkconfig.h

gdkenumtypes.h: stamp-gdkenumtypes.h
	@true
stamp-gdkenumtypes.h: @REBUILD@ $(gdk_public_h_sources) gdkenumtypes.h.template
	( cd $(srcdir) && $(GLIB_MKENUMS) --template gdkenumtypes.h.template \
		$(gdk_public_h_sources) ) >> xgen-geth \
	&& (cmp -s xgen-geth gdkenumtypes.h || cp xgen-geth gdkenumtypes.h ) \
	&& rm -f xgen-geth \
	&& echo timestamp > $(@F)
gdkenumtypes.c: @REBUILD@ $(gdk_public_h_sources) gdkenumtypes.c.template
	( cd $(srcdir) && $(GLIB_MKENUMS) --template gdkenumtypes.c.template \
		$(gdk_public_h_sources) ) > xgen-getc \
	&& cp xgen-getc gdkenumtypes.c  \
	&& rm -f xgen-getc


gdkconfig.h: stamp-gc-h
	@if test -f gdkconfig.h; then :; \
	else rm -f stamp-gc-h; $(MAKE) stamp-gc-h; fi
stamp-gc-h: $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status gdk/gdkconfig.h
	echo timestamp > stamp-gc-h

dist-hook:	\
	$(INTROSPECTION_INTERMEDIATE_ITEMS)

DISTCLEANFILES = gdkconfig.h stamp-gc-h

uninstall-local:
	rm -f $(DESTDIR)$(configexecincludedir)/gdkconfig.h
.PHONY: uninstall-local

# if srcdir!=builddir, clean out maintainer-clean files from builddir
# this allows dist to pass.
distclean-local:
	if test $(srcdir) != .; then \
	  rm -f $(MAINTAINERCLEANFILES); \
	fi

.PHONY: files

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done

-include $(top_srcdir)/git.mk
