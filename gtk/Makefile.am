# Makefile.am for gtk+/gtk
include $(top_srcdir)/Makefile.decl
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = \
	--warn-all \
	--add-include-path=../gdk
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir) \
    --includedir=. \
    --includedir=../gdk

GTK_PRINT_PREVIEW_COMMAND="evince --unlink-tempfile --preview --print-settings %s %f"

SUBDIRS = .
DIST_SUBDIRS=

if BUILD_TESTS
SUBDIRS += tests
DIST_SUBDIRS += tests
endif

if HAVE_PRINT
if HAVE_PAPI_CUPS
GTK_PRINT_BACKENDS=file,papi,cups
else
if HAVE_CUPS
GTK_PRINT_BACKENDS=file,cups
else
if HAVE_PAPI
GTK_PRINT_BACKENDS=file,papi
else
GTK_PRINT_BACKENDS=file,lpr
endif
endif
endif
else
GTK_PRINT_BACKENDS=
endif

# manually remove if wanted
# grep for DISABLE_PRINTING_BACKENDS to easly find
#GTK_PRINT_BACKENDS=

# disable print backends

GTK_BASE_CFLAGS_DEFINES =				\
	-DGTK_VERSION=\"$(GTK_VERSION)\"		\
	-DGTK_BINARY_VERSION=\"$(GTK_BINARY_VERSION)\"	\
	-DGTK_COMPILATION				\
	$(/bin/true)			\
	-DGTK_FILE_SYSTEM_ENABLE_UNSUPPORTED		\
	-DGTK_PRINT_BACKEND_ENABLE_UNSUPPORTED

GTK_PLAT_CFLAGS_DEFINES =				\
	-DGTK_LIBDIR=\"$(libdir)\"			\
	-DGTK_DATADIR=\"$(datadir)\"			\
	-DGTK_DATA_PREFIX=\"$(prefix)\"			\
	-DGTK_SYSCONFDIR=\"$(sysconfdir)\"		\
	-DGTK_HOST=\"$(host)\"				\
	-DGTK_PRINT_BACKENDS=\"$(GTK_PRINT_BACKENDS)\"	\
	-DGTK_PRINT_PREVIEW_COMMAND=\"$(GTK_PRINT_PREVIEW_COMMAND)\"

INCLUDES =						\
	$(GTK_BASE_CFLAGS_DEFINES)			\
	$(GTK_PLAT_CFLAGS_DEFINES)			\
	-DG_LOG_DOMAIN=\"Gtk\"				\
	-I$(top_builddir)				\
	-I$(top_builddir)/gtk				\
	-I$(top_srcdir) -I../gdk			\
	-I$(top_srcdir)/gdk				\
	$(GMODULE_CFLAGS)				\
	$(GTK_DEBUG_FLAGS)				\
	$(GTK_DEP_CFLAGS)

gtarget=$(gdktarget)

TESTS_ENVIRONMENT = srcdir="$(srcdir)" gtk_all_c_sources="$(gtk_all_c_sources)"

libgtkincludedir = $(includedir)/gtk-2.0/gtk
libadd =								\
	$(top_builddir)/gdk/$(gdktargetlib)				\
	$(GMODULE_LIBS) \
	$(GTK_DEP_LIBS)
deps =

# libtool stuff: set version and export symbols for resolving
# since automake doesn't support conditionalized libsomething_la_LDFLAGS
# we use the general approach here
libtool_opts =							\
  -version-info $(LT_VERSION_INFO)				\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)	\
  -rpath $(libdir) $(libgtk_target_ldflags)


#
# setup source file variables
#

# GTK+ header files for public installation (non-generated, or generated
# by configure)
gtk_public_h_sources =          \
	gtk.h			\
	gtkaboutdialog.h	\
	gtkaccelgroup.h		\
	gtkaccellabel.h		\
	gtkaccelmap.h		\
	gtkaccessible.h		\
	gtkaction.h		\
	gtkactiongroup.h	\
	gtkactivatable.h	\
	gtkadjustment.h		\
	gtkalignment.h		\
	gtkarrow.h		\
	gtkaspectframe.h	\
	gtkassistant.h		\
	gtkbbox.h		\
	gtkbin.h		\
	gtkbindings.h		\
	gtkbox.h		\
	gtkbuilder.h		\
	gtkbuildable.h		\
	gtkbutton.h		\
	gtkcalendar.h		\
	gtkcelleditable.h	\
	gtkcelllayout.h		\
	gtkcellrenderer.h	\
	gtkcellrendereraccel.h	\
	gtkcellrenderercombo.h	\
	gtkcellrendererpixbuf.h	\
	gtkcellrendererprogress.h \
	gtkcellrendererspin.h   \
	gtkcellrendererspinner.h\
	gtkcellrenderertext.h	\
	gtkcellrenderertoggle.h	\
	gtkcellview.h		\
	gtkcheckbutton.h	\
	gtkcheckmenuitem.h	\
	gtkclipboard.h		\
	gtkcolorbutton.h	\
	gtkcolorsel.h		\
	gtkcolorseldialog.h	\
	gtkcombobox.h		\
	gtkcomboboxentry.h	\
	gtkcomboboxtext.h	\
	gtkcontainer.h		\
	gtkdebug.h              \
	gtkdialog.h		\
	gtkdnd.h		\
	gtkdrawingarea.h	\
	gtkeditable.h           \
	gtkentry.h		\
	gtkentrybuffer.h	\
	gtkentrycompletion.h	\
	gtkenums.h		\
	gtkeventbox.h		\
	gtkexpander.h		\
	gtkfilechooser.h        \
	gtkfilechooserbutton.h  \
	gtkfilechooserdialog.h  \
	gtkfilechooserwidget.h  \
	gtkfilefilter.h		\
	gtkfixed.h		\
	gtkfontbutton.h		\
	gtkfontsel.h		\
	gtkframe.h		\
	gtkgc.h			\
	gtkhandlebox.h		\
	gtkhbbox.h		\
	gtkhbox.h		\
	gtkhpaned.h		\
	gtkhruler.h		\
	gtkhscale.h		\
	gtkhscrollbar.h		\
	gtkhseparator.h		\
	gtkhsv.h		\
	gtkiconfactory.h	\
	gtkicontheme.h		\
	gtkiconview.h		\
	gtkimage.h		\
	gtkimagemenuitem.h	\
	gtkimcontext.h		\
	gtkimcontextsimple.h	\
	gtkimmulticontext.h	\
	gtkinfobar.h		\
	gtkinvisible.h		\
	gtkitem.h		\
	gtklabel.h		\
	gtklayout.h             \
	gtklinkbutton.h		\
	gtkliststore.h		\
	gtkmain.h		\
	gtkmenu.h		\
	gtkmenubar.h		\
	gtkmenuitem.h		\
	gtkmenushell.h		\
	gtkmenutoolbutton.h	\
	gtkmessagedialog.h	\
	gtkmisc.h		\
	gtkmodules.h		\
	gtkmountoperation.h     \
	gtknotebook.h		\
	gtkobject.h		\
	gtkoffscreenwindow.h	\
	gtkorientable.h		\
	gtkpagesetup.h		\
	gtkpaned.h		\
	gtkpapersize.h		\
	gtkplug.h		\
	gtkprintcontext.h	\
	gtkprintoperation.h	\
	gtkprintoperationpreview.h	\
	gtkprintsettings.h	\
	gtkprivate.h		\
	gtkprogressbar.h	\
	gtkradioaction.h	\
	gtkradiobutton.h	\
	gtkradiomenuitem.h	\
	gtkradiotoolbutton.h	\
	gtkrange.h		\
	gtkrc.h			\
	gtkrecentaction.h	\
	gtkrecentchooser.h 	\
	gtkrecentchooserdialog.h \
	gtkrecentchoosermenu.h 	\
	gtkrecentchooserwidget.h \
	gtkrecentfilter.h 	\
	gtkrecentmanager.h 	\
	gtkruler.h		\
	gtkscale.h		\
	gtkscalebutton.h	\
	gtkscrollbar.h		\
	gtkscrolledwindow.h	\
	gtkselection.h		\
	gtkseparator.h		\
	gtkseparatormenuitem.h	\
	gtkseparatortoolitem.h	\
	gtkshow.h		\
	gtksettings.h		\
	gtksizegroup.h		\
	gtksocket.h		\
	gtkspinbutton.h		\
	gtkspinner.h		\
	gtkstatusbar.h		\
	gtkstatusicon.h		\
	gtkstock.h		\
	gtkstyle.h		\
	gtktable.h		\
	gtktearoffmenuitem.h    \
	gtktestutils.h		\
	gtktextbuffer.h		\
	gtktextbufferrichtext.h	\
	gtktextchild.h		\
	gtktextdisplay.h	\
	gtktextiter.h		\
	gtktextmark.h		\
	gtktexttag.h		\
	gtktexttagtable.h	\
	gtktextview.h		\
	gtktoggleaction.h	\
	gtktogglebutton.h	\
	gtktoggletoolbutton.h	\
	gtktoolbar.h		\
	gtktoolbutton.h		\
	gtktoolitem.h		\
	gtktoolitemgroup.h	\
	gtktoolpalette.h	\
	gtktoolshell.h		\
	gtktooltip.h		\
	gtktreednd.h		\
	gtktreemodel.h		\
	gtktreemodelfilter.h	\
	gtktreemodelsort.h	\
	gtktreeselection.h	\
	gtktreesortable.h	\
	gtktreestore.h		\
	gtktreeview.h		\
	gtktreeviewcolumn.h	\
	gtktypeutils.h		\
	gtkuimanager.h		\
	gtkvbbox.h		\
	gtkvbox.h		\
	gtkviewport.h		\
	gtkvolumebutton.h	\
	gtkvpaned.h		\
	gtkvruler.h		\
	gtkvscale.h		\
	gtkvscrollbar.h		\
	gtkvseparator.h		\
	gtkwidget.h		\
	gtkwindow.h

gtk_unix_print_public_h_sources =    \
	gtkpagesetupunixdialog.h	\
	gtkprintunixdialog.h		\
	gtkprinter.h			\
	gtkprintjob.h			\
	gtkunixprint.h

# Installed header files without compatibility guarantees
# that are not included in gtk/gtk.h
gtk_semi_private_h_sources =    \
	gtktextlayout.h

# GTK+ header files that don't get installed
gtk_private_h_sources =		\
	gtkquery.h		\
	gtksearchengine.h	\
	gtksearchenginesimple.h	\
	gtkdndcursors.h		\
	gtkentryprivate.h	\
	gtkbuilderprivate.h 	\
	gtkcustompaperunixdialog.h\
	gtkfilechooserdefault.h	\
	gtkfilechooserembed.h	\
	gtkfilechooserentry.h	\
	gtkfilechooserprivate.h	\
	gtkfilechoosersettings.h \
	gtkfilechooserutils.h	\
	gtkfilesystem.h		\
	gtkfilesystemmodel.h	\
	gtkiconcache.h		\
	gtkintl.h		\
	gtkkeyhash.h		\
	gtkmnemonichash.h	\
	gtkmountoperationprivate.h \
	gtkpango.h		\
	gtkpathbar.h		\
	gtkplugprivate.h	\
	gtkprintoperation-private.h\
	gtkprintutils.h		\
	gtkrbtree.h		\
	gtkrecentchooserdefault.h \
	gtkrecentchooserprivate.h \
	gtkrecentchooserutils.h \
	gtksocketprivate.h	\
	gtktextbtree.h		\
	gtktextbufferserialize.h\
	gtktextchildprivate.h	\
	gtktextiterprivate.h	\
	gtktextmarkprivate.h	\
	gtktextsegment.h	\
	gtktexttagprivate.h	\
	gtktexttypes.h		\
	gtktextutil.h		\
	gtkthemes.h		\
	gtktoggleactionprivate.h\
	gtktoolpaletteprivate.h	\
	gtktreedatalist.h	\
	gtktreeprivate.h	\
	gtkwindow-decorate.h	\
	$(gtk_clipboard_dnd_h_sources)

# GTK+ C sources to build the library from
gtk_base_c_sources =            \
	gtkquery.c		\
	gtksearchengine.c	\
	gtksearchenginesimple.c	\
	fnmatch.c		\
	gtkaboutdialog.c	\
	gtkaccelgroup.c		\
	gtkaccellabel.c		\
	gtkaccelmap.c		\
	gtkaccessible.c		\
	gtkaction.c		\
	gtkactiongroup.c	\
	gtkactivatable.c	\
	gtkadjustment.c		\
	gtkalignment.c		\
	gtkarrow.c		\
	gtkaspectframe.c	\
	gtkassistant.c		\
	gtkbbox.c		\
	gtkbin.c		\
	gtkbindings.c		\
	gtkbox.c		\
	gtkbuildable.c		\
	gtkbuilder.c		\
	gtkbuilderparser.c	\
	gtkbutton.c		\
	gtkcalendar.c		\
	gtkcelleditable.c	\
	gtkcelllayout.c		\
	gtkcellrenderer.c	\
	gtkcellrendereraccel.c	\
	gtkcellrenderercombo.c	\
	gtkcellrendererpixbuf.c	\
	gtkcellrendererprogress.c \
	gtkcellrendererspin.c   \
	gtkcellrendererspinner.c\
	gtkcellrenderertext.c	\
	gtkcellrenderertoggle.c	\
	gtkcellview.c		\
	gtkcheckbutton.c	\
	gtkcheckmenuitem.c	\
	gtkcolorbutton.c	\
	gtkcolorsel.c		\
	gtkcolorseldialog.c	\
	gtkcombobox.c		\
	gtkcomboboxentry.c	\
	gtkcomboboxtext.c 	\
	gtkcontainer.c		\
	gtkdialog.c		\
	gtkdrawingarea.c	\
	gtkeditable.c           \
	gtkentry.c		\
	gtkentrybuffer.c	\
	gtkentrycompletion.c	\
	gtkeventbox.c		\
	gtkexpander.c		\
	gtkfilechooser.c	\
	gtkfilechooserbutton.c	\
	gtkfilechooserdefault.c	\
	gtkfilechooserdialog.c	\
	gtkfilechooserembed.c	\
	gtkfilechooserentry.c	\
	gtkfilechoosersettings.c \
	gtkfilechooserutils.c	\
	gtkfilechooserwidget.c	\
	gtkfilefilter.c		\
	gtkfilesystem.c		\
	gtkfilesystemmodel.c	\
	gtkfixed.c		\
	gtkfontbutton.c         \
	gtkfontsel.c            \
	gtkframe.c		\
	gtkgc.c			\
	gtkhandlebox.c		\
	gtkhbbox.c		\
	gtkhbox.c		\
	gtkhpaned.c		\
	gtkhruler.c		\
	gtkhscale.c		\
	gtkhscrollbar.c		\
	gtkhseparator.c		\
	gtkhsv.c		\
	gtkiconcache.c		\
	gtkiconcachevalidator.c	\
	gtkiconfactory.c	\
	gtkicontheme.c		\
	gtkiconview.c		\
	gtkimage.c		\
	gtkimagemenuitem.c	\
	gtkimcontext.c		\
	gtkimcontextsimple.c	\
	gtkimmulticontext.c	\
	gtkinfobar.c		\
	gtkinputdialog.c	\
	gtkinvisible.c		\
	gtkitem.c		\
	gtkkeyhash.c		\
	gtklabel.c		\
	gtklayout.c		\
	gtklinkbutton.c		\
	gtkliststore.c		\
	gtkmain.c		\
	gtkmenu.c		\
	gtkmenubar.c		\
	gtkmenuitem.c		\
	gtkmenushell.c		\
	gtkmenutoolbutton.c	\
	gtkmessagedialog.c	\
	gtkmisc.c		\
	gtkmnemonichash.c	\
	gtkmodules.c		\
	gtkmountoperation.c     \
	gtknotebook.c		\
	gtkobject.c		\
	gtkoffscreenwindow.c	\
	gtkorientable.c		\
	gtkpagesetup.c		\
	gtkpaned.c		\
	gtkpango.c		\
	gtkpapersize.c		\
	gtkpathbar.c		\
	gtkplug.c		\
	gtkprintcontext.c	\
	gtkprintoperation.c	\
	gtkprintoperationpreview.c	\
	gtkprintsettings.c	\
	gtkprintutils.c		\
	gtkprogressbar.c	\
	gtkradioaction.c	\
	gtkradiobutton.c	\
	gtkradiomenuitem.c	\
	gtkradiotoolbutton.c	\
	gtkrange.c		\
	gtkrbtree.c 		\
	gtkrc.c			\
	gtkrecentaction.c	\
	gtkrecentchooserdefault.c \
	gtkrecentchooserdialog.c \
	gtkrecentchoosermenu.c 	\
	gtkrecentchooserwidget.c \
	gtkrecentchooserutils.c \
	gtkrecentchooser.c 	\
	gtkrecentfilter.c 	\
	gtkrecentmanager.c 	\
	gtkruler.c		\
	gtkscale.c		\
	gtkscalebutton.c	\
	gtkscrollbar.c		\
	gtkscrolledwindow.c	\
	gtkselection.c		\
	gtkseparator.c		\
	gtkseparatormenuitem.c	\
	gtkseparatortoolitem.c	\
	gtksettings.c		\
	gtksizegroup.c		\
	gtkshow.c		\
	gtksocket.c		\
	gtkspinbutton.c		\
	gtkspinner.c		\
	gtkstatusbar.c		\
	gtkstatusicon.c		\
	gtkstock.c		\
	gtkstyle.c		\
	gtktable.c		\
	gtktearoffmenuitem.c    \
	gtktestutils.c		\
	gtktextbtree.c		\
	gtktextbuffer.c		\
	gtktextbufferrichtext.c	\
	gtktextbufferserialize.c\
	gtktextchild.c		\
	gtktextdisplay.c	\
	gtktextiter.c		\
	gtktextlayout.c		\
	gtktextmark.c		\
	gtktextsegment.c	\
	gtktexttag.c		\
	gtktexttagtable.c	\
	gtktexttypes.c		\
	gtktextutil.c		\
	gtktextview.c		\
	gtkthemes.c		\
	gtktoggleaction.c	\
	gtktogglebutton.c	\
	gtktoggletoolbutton.c	\
	gtktoolbar.c		\
	gtktoolbutton.c		\
	gtktoolitem.c		\
	gtktoolitemgroup.c		\
	gtktoolpalette.c	\
	gtktoolshell.c		\
	gtktooltip.c		\
	gtktreedatalist.c	\
	gtktreednd.c		\
	gtktreemodel.c		\
	gtktreemodelfilter.c	\
	gtktreemodelsort.c	\
	gtktreeselection.c	\
	gtktreesortable.c	\
	gtktreestore.c		\
	gtktreeview.c		\
	gtktreeviewcolumn.c	\
	gtktypebuiltins.c	\
	gtktypeutils.c		\
	gtkuimanager.c		\
	gtkvbbox.c		\
	gtkvbox.c		\
	gtkvolumebutton.c	\
	gtkviewport.c		\
	gtkvpaned.c		\
	gtkvruler.c		\
	gtkvscale.c		\
	gtkvscrollbar.c		\
	gtkvseparator.c		\
	gtkwidget.c		\
	gtkwindow-decorate.c    \
	gtkwindow.c		\
	$(gtk_clipboard_dnd_c_sources)

# Broken
gtk_public_h_sources += \
	gtktext.h		\
	gtktree.h		\
	gtktreeitem.h
gtk_base_c_sources += \
	gtktext.c		\
	gtktree.c		\
	gtktreeitem.c

# Deprecated
gtk_public_h_sources += \
	gtkclist.h		\
	gtkcombo.h		\
	gtkctree.h		\
	gtkcurve.h		\
	gtkfilesel.h		\
	gtkgamma.h		\
	gtkinputdialog.h	\
	gtkitemfactory.h	\
	gtklist.h		\
	gtklistitem.h		\
	gtkoldeditable.h	\
	gtkoptionmenu.h		\
	gtkpixmap.h		\
	gtkpreview.h		\
	gtkprogress.h		\
	gtksignal.h		\
	gtktipsquery.h		\
	gtktooltips.h
gtk_base_c_sources += \
	gtkclist.c		\
	gtkcombo.c		\
	gtkctree.c		\
	gtkcurve.c		\
	gtkfilesel.c		\
	gtkgamma.c		\
	gtkitemfactory.c	\
	gtklist.c		\
	gtklistitem.c		\
	gtkoldeditable.c	\
	gtkoptionmenu.c		\
	gtkpixmap.c		\
	gtkpreview.c		\
	gtkprogress.c		\
	gtksignal.c		\
	gtktipsquery.c		\
	gtktooltips.c

gtk_c_sources = $(gtk_base_c_sources)
gtk_all_c_sources = $(gtk_base_c_sources)

gtk_os_unix_c_sources = \
	gtkcustompaperunixdialog.c	\
	gtkpagesetupunixdialog.c	\
	gtkprinter.c			\
	gtkprinteroption.c		\
	gtkprinteroptionset.c		\
	gtkprinteroptionwidget.c	\
	gtkprintjob.c			\
	gtkprintoperation-unix.c	\
	gtkprintunixdialog.c		\
        gtkprintbackend.c		\
	gtksearchenginebeagle.c		\
	gtksearchenginetracker.c
gtk_all_c_sources += $(gtk_os_unix_c_sources)

gtk_private_h_sources += \
	gtkiconcachevalidator.h		\
	gtkprintbackend.h		\
	gtkprinter-private.h		\
	gtkprinteroption.h		\
	gtkprinteroptionset.h		\
	gtkprinteroptionwidget.h	\
	gtksearchenginebeagle.h		\
	gtksearchenginetracker.h
gtk_c_sources += $(gtk_os_unix_c_sources)


gtk_use_x11_c_sources = \
	gtkplug-x11.c   \
	gtksocket-x11.c \
	gtkxembed.c     \
	gtktrayicon-x11.c \
	gtkmountoperation-x11.c
gtk_use_stub_c_sources = \
	gtkplug-stub.c   \
	gtksocket-stub.c \
	gtkmountoperation-stub.c
gtk_all_c_sources += $(gtk_use_x11_c_sources) $(gtk_use_stub_c_sources)
if USE_X11
gtk_private_h_sources += gtkxembed.h gtktrayicon.h xembed.h
gtk_c_sources += $(gtk_use_x11_c_sources)
else
gtk_c_sources += $(gtk_use_stub_c_sources)
endif

gtk_clipboard_dnd_c_sources = gtkclipboard.c gtkdnd.c

# we use our own built_sources variable rules to avoid automake's
# BUILT_SOURCES oddities
# we generate frequently rebuild files piggyback on a stamp file, so sources
# depending on them only get rebuild when the built source actually changed
# content

# built sources that get installed with the header files
gtk_built_public_sources =			\
	gtktypebuiltins.h

# built headers that don't get installed
gtk_built_private_headers =			\
	gtkbuiltincache.h

gtk_built_sources =				\
	gtktypebuiltins.c			\
	gtktypefuncs.c				\
	${gtk_built_private_headers}		\
	${gtk_built_public_sources}

stamp_files =					\
	stamp-gtktypebuiltins.h			\
	stamp-icons

# non-header sources (headers should be specified in the above variables)
# that don't serve as direct make target sources, i.e. they don't have
# their own .lo rules and don't get publically installed
gtk_extra_sources =				\
	paper_names.c				\
	paper_names_offsets.c			\
	gen-paper-names.c			\
	gtkversion.h.in

#
# setup GTK+ sources and their dependencies
#
MAINTAINERCLEANFILES = $(gtk_built_sources) $(stamp_files) $(GENERATED_ICONS) stock-icons/icon-theme.cache
EXTRA_HEADERS =
EXTRA_DIST += $(gtk_private_h_sources) $(gtk_extra_sources)
EXTRA_DIST += $(gtk_built_sources)
EXTRA_DIST += $(STOCK_ICONS)

#
# rules to generate built sources
#
# setup autogeneration dependencies
gen_sources = xgen-gdef xgen-gtbh xgen-gtic xgen-gmh xgen-gmc xgen-gmlh xgen-gmlc xgen-gtfsrc.c xgen-gtf
CLEANFILES = $(gen_sources) $(gtk_def)

BUILT_SOURCES = $(gtk_built_sources)

gtktypebuiltins.h: stamp-gtktypebuiltins.h
	@true
stamp-gtktypebuiltins.h: @REBUILD@ $(gtk_public_h_sources) gtktypebuiltins.h.template
	( cd $(srcdir) && $(GLIB_MKENUMS) --template gtktypebuiltins.h.template \
		$(gtk_public_h_sources) ) >> xgen-gtbh \
	&& (cmp -s xgen-gtbh gtktypebuiltins.h || cp xgen-gtbh gtktypebuiltins.h ) \
	&& rm -f xgen-gtbh \
	&& echo timestamp > $(@F)
gtktypebuiltins.c: @REBUILD@ $(gtk_public_h_sources) gtktypebuiltins.c.template
	( cd $(srcdir) && $(GLIB_MKENUMS) --template gtktypebuiltins.c.template \
		$(gtk_public_h_sources) ) > xgen-gtbc \
	&& cp xgen-gtbc gtktypebuiltins.c  \
	&& rm -f xgen-gtbc

# target platform:
lib_LTLIBRARIES = $(gtktargetlib)

gtkincludedir = $(includedir)/gtk-2.0/gtk
gtkinclude_HEADERS = $(gtk_public_h_sources) $(gtk_semi_private_h_sources) $(gtk_built_public_sources) gtkversion.h

gtkunixprintincludedir = $(includedir)/gtk-unix-print-2.0/gtk
gtkunixprintinclude_HEADERS = $(gtk_unix_print_public_h_sources)

libgtk_x11_2_0_la_SOURCES = $(gtk_c_sources)
libgtk_directfb_2_0_la_SOURCES = $(gtk_c_sources)

libgtk_x11_2_0_la_LDFLAGS = $(libtool_opts)
libgtk_directfb_2_0_la_LDFLAGS = $(libtool_opts)

libgtk_x11_2_0_la_LIBADD = $(libadd)
libgtk_directfb_2_0_la_LIBADD = $(libadd)

libgtk_x11_2_0_la_DEPENDENCIES = $(deps)
libgtk_directfb_2_0_la_DEPENDENCIES = $(deps)

EXTRA_LTLIBRARIES = libgtk-x11-2.0.la libgtk-directfb-2.0.la

install-exec-hook: 
if DISABLE_EXPLICIT_DEPS
	$(SHELL) $(top_srcdir)/sanitize-la.sh $(DESTDIR)$(libdir)/$(gtktargetlib)
endif

dist-hook:	\
	$(INTROSPECTION_INTERMEDIATE_ITEMS)

# Install a RC file for the default GTK+ theme, and key themes
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datadir)/themes/Raleigh/gtk-2.0
	$(INSTALL_DATA) $(srcdir)/gtkrc.default $(DESTDIR)$(datadir)/themes/Raleigh/gtk-2.0/gtkrc
	$(mkinstalldirs) $(DESTDIR)$(datadir)/themes/Default/gtk-2.0-key
	$(INSTALL_DATA) $(srcdir)/gtkrc.key.default $(DESTDIR)$(datadir)/themes/Default/gtk-2.0-key/gtkrc
	$(mkinstalldirs) $(DESTDIR)$(datadir)/themes/Emacs/gtk-2.0-key
	$(INSTALL_DATA) $(srcdir)/gtkrc.key.emacs $(DESTDIR)$(datadir)/themes/Emacs/gtk-2.0-key/gtkrc

uninstall-local:
	rm -f $(DESTDIR)$(datadir)/themes/Raleigh/gtk-2.0/gtkrc
	rm -f $(DESTDIR)$(datadir)/themes/Default/gtk-2.0-key/gtkrc
	rm -f $(DESTDIR)$(datadir)/themes/Emacs/gtk-2.0-key/gtkrc

.PHONY: install-data-local uninstall-local

# if srcdir!=builddir, clean out maintainer-clean files from builddir
# this allows dist to pass.
distclean-local:
	if test $(srcdir) != .; then \
	  rm -f $(MAINTAINERCLEANFILES); \
	fi

DEPS = $(gtktargetlib) $(top_builddir)/gdk/$(gdktargetlib)

TEST_DEPS = $(DEPS) immodules.cache

LDADDS =								\
	$(gtktargetlib)							\
	$(top_builddir)/gdk/$(gdktargetlib)                             \
	$(GTK_DEP_LIBS)

if HAVE_INTROSPECTION
introspection_files = \
    $(gtkinclude_HEADERS) \
    $(gtk_all_c_sources) \
    gtktypebuiltins.h \
    gtktypebuiltins.c

Gtk-2.0.gir: $(INTROSPECTION_SCANNER) $(gtktargetlib) $(top_builddir)/gdk/Gdk-2.0.gir Makefile
Gtk_2_0_gir_SCANNERFLAGS = --include-uninstalled=$(top_builddir)/gdk/Gdk-2.0.gir
if USE_X11
Gtk_2_0_gir_SCANNERFLAGS += --add-include-path=$(top_builddir)/gdk/x11
endif
Gtk_2_0_gir_INCLUDES =
Gtk_2_0_gir_CFLAGS = \
		$(INCLUDES) \
		-UGDK_DISABLE_DEPRECATED \
		-UGTK_DISABLE_DEPRECATED \
		-DGTK_TEXT_USE_INTERNAL_UNSUPPORTED_API
Gtk_2_0_gir_LIBS = $(gtktargetlib)
Gtk_2_0_gir_FILES = $(introspection_files)
Gtk_2_0_gir_EXPORT_PACKAGES = gtk+-2.0
INTROSPECTION_GIRS += Gtk-2.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)
endif

#
# Installed tools
#
bin_PROGRAMS =

bin_SCRIPTS =

.PHONY: files test test-debug

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done

STOCK_ICONS = \
	stock-icons/16/application-exit.png		\
	stock-icons/16/dialog-information.png		\
	stock-icons/16/document-new.png			\
	stock-icons/16/document-open.png		\
	stock-icons/16/document-open-recent.png		\
	stock-icons/16/document-print.png		\
	stock-icons/16/document-print-preview.png	\
	stock-icons/16/document-properties.png		\
	stock-icons/16/document-revert-ltr.png		\
	stock-icons/16/document-revert-rtl.png		\
	stock-icons/16/document-save-as.png		\
	stock-icons/16/drive-harddisk.png		\
	stock-icons/16/edit-clear.png			\
	stock-icons/16/edit-copy.png			\
	stock-icons/16/edit-cut.png			\
	stock-icons/16/edit-delete.png			\
	stock-icons/16/edit-find.png			\
	stock-icons/16/edit-find-replace.png		\
	stock-icons/16/edit-paste.png			\
	stock-icons/16/edit-redo-ltr.png		\
	stock-icons/16/edit-redo-rtl.png		\
	stock-icons/16/edit-select-all.png		\
	stock-icons/16/edit-undo-ltr.png		\
	stock-icons/16/edit-undo-rtl.png		\
	stock-icons/16/folder.png			\
	stock-icons/16/format-indent-less-ltr.png	\
	stock-icons/16/format-indent-less-rtl.png	\
	stock-icons/16/format-indent-more-ltr.png	\
	stock-icons/16/format-indent-more-rtl.png	\
	stock-icons/16/format-justify-center.png	\
	stock-icons/16/format-justify-fill.png		\
	stock-icons/16/format-justify-left.png		\
	stock-icons/16/format-justify-right.png		\
	stock-icons/16/format-text-bold.png		\
	stock-icons/16/format-text-italic.png		\
	stock-icons/16/format-text-strikethrough.png	\
	stock-icons/16/format-text-underline.png	\
	stock-icons/16/go-previous-ltr.png		\
	stock-icons/16/go-bottom.png			\
	stock-icons/16/go-down.png			\
	stock-icons/16/go-first-ltr.png			\
	stock-icons/16/go-home.png			\
	stock-icons/16/go-jump-ltr.png			\
	stock-icons/16/go-jump-rtl.png			\
	stock-icons/16/go-last-ltr.png			\
	stock-icons/16/go-next-ltr.png			\
	stock-icons/16/go-previous-ltr.png		\
	stock-icons/16/go-top.png			\
	stock-icons/16/go-up.png			\
	stock-icons/16/gtk-caps-lock-warning.png	\
	stock-icons/16/gtk-color-picker.png		\
	stock-icons/16/gtk-connect.png			\
	stock-icons/16/gtk-convert.png			\
	stock-icons/16/gtk-disconnect.png		\
	stock-icons/16/gtk-edit.png			\
	stock-icons/16/gtk-font.png			\
	stock-icons/16/gtk-index.png			\
	stock-icons/16/gtk-orientation-landscape.png	\
	stock-icons/16/gtk-orientation-portrait.png	\
	stock-icons/16/gtk-orientation-reverse-landscape.png \
	stock-icons/16/gtk-orientation-reverse-portrait.png \
	stock-icons/16/gtk-page-setup.png		\
	stock-icons/16/gtk-preferences.png		\
	stock-icons/16/gtk-select-color.png		\
	stock-icons/16/gtk-select-font.png		\
	stock-icons/16/gtk-undelete-ltr.png		\
	stock-icons/16/gtk-undelete-rtl.png		\
	stock-icons/16/help-about.png			\
	stock-icons/16/help-contents.png		\
	stock-icons/16/image-missing.png		\
	stock-icons/16/list-add.png			\
	stock-icons/16/list-remove.png			\
	stock-icons/16/media-floppy.png			\
	stock-icons/16/media-optical.png		\
	stock-icons/16/media-playback-pause.png		\
	stock-icons/16/media-playback-start-ltr.png	\
	stock-icons/16/media-playback-start-rtl.png	\
	stock-icons/16/media-playback-stop.png		\
	stock-icons/16/media-record.png			\
	stock-icons/16/media-seek-backward-ltr.png	\
	stock-icons/16/media-seek-forward-ltr.png	\
	stock-icons/16/media-skip-backward-ltr.png	\
	stock-icons/16/media-skip-forward-ltr.png	\
	stock-icons/16/network-idle.png			\
	stock-icons/16/printer-error.png		\
	stock-icons/16/printer-info.png			\
	stock-icons/16/printer-paused.png		\
	stock-icons/16/printer-warning.png		\
	stock-icons/16/process-stop.png			\
	stock-icons/16/system-run.png			\
	stock-icons/16/text-x-generic.png		\
	stock-icons/16/tools-check-spelling.png		\
	stock-icons/16/view-fullscreen.png		\
	stock-icons/16/view-refresh.png			\
	stock-icons/16/view-restore.png			\
	stock-icons/16/view-sort-ascending.png		\
	stock-icons/16/view-sort-descending.png		\
	stock-icons/16/window-close.png			\
	stock-icons/16/zoom-fit-best.png		\
	stock-icons/16/zoom-in.png			\
	stock-icons/16/zoom-original.png		\
	stock-icons/16/zoom-out.png			\
	\
	stock-icons/20/gtk-apply.png			\
	stock-icons/20/gtk-cancel.png			\
	stock-icons/20/gtk-no.png			\
	stock-icons/20/gtk-ok.png			\
	stock-icons/20/gtk-yes.png			\
	stock-icons/20/window-close.png			\
	\
	stock-icons/24/application-exit.png		\
	stock-icons/24/audio-volume-high.png		\
	stock-icons/24/audio-volume-low.png		\
	stock-icons/24/audio-volume-medium.png		\
	stock-icons/24/audio-volume-muted.png		\
	stock-icons/24/dialog-information.png		\
	stock-icons/24/document-new.png			\
	stock-icons/24/document-open.png		\
	stock-icons/24/document-open-recent.png		\
	stock-icons/24/document-print.png		\
	stock-icons/24/document-print-preview.png	\
	stock-icons/24/document-properties.png		\
	stock-icons/24/document-revert-ltr.png		\
	stock-icons/24/document-revert-rtl.png		\
	stock-icons/24/document-save-as.png		\
	stock-icons/24/drive-harddisk.png		\
	stock-icons/24/edit-clear.png			\
	stock-icons/24/edit-copy.png			\
	stock-icons/24/edit-cut.png			\
	stock-icons/24/edit-delete.png			\
	stock-icons/24/edit-find.png			\
	stock-icons/24/edit-find-replace.png		\
	stock-icons/24/edit-paste.png			\
	stock-icons/24/edit-redo-ltr.png		\
	stock-icons/24/edit-redo-rtl.png		\
	stock-icons/24/edit-select-all.png		\
	stock-icons/24/edit-undo-ltr.png		\
	stock-icons/24/edit-undo-rtl.png		\
	stock-icons/24/folder.png			\
	stock-icons/24/format-indent-less-ltr.png	\
	stock-icons/24/format-indent-less-rtl.png	\
	stock-icons/24/format-indent-more-ltr.png	\
	stock-icons/24/format-indent-more-rtl.png	\
	stock-icons/24/format-justify-center.png	\
	stock-icons/24/format-justify-fill.png		\
	stock-icons/24/format-justify-left.png		\
	stock-icons/24/format-justify-right.png		\
	stock-icons/24/format-text-bold.png		\
	stock-icons/24/format-text-italic.png		\
	stock-icons/24/format-text-strikethrough.png	\
	stock-icons/24/format-text-underline.png	\
	stock-icons/24/go-bottom.png			\
	stock-icons/24/go-down.png			\
	stock-icons/24/go-first-ltr.png			\
	stock-icons/24/go-home.png			\
	stock-icons/24/go-jump-ltr.png			\
	stock-icons/24/go-jump-rtl.png			\
	stock-icons/24/go-last-ltr.png			\
	stock-icons/24/go-next-ltr.png			\
	stock-icons/24/go-previous-ltr.png		\
	stock-icons/24/go-top.png			\
	stock-icons/24/go-up.png			\
	stock-icons/24/gtk-caps-lock-warning.png	\
	stock-icons/24/gtk-color-picker.png		\
	stock-icons/24/gtk-connect.png			\
	stock-icons/24/gtk-convert.png			\
	stock-icons/24/gtk-disconnect.png		\
	stock-icons/24/gtk-edit.png			\
	stock-icons/24/gtk-font.png			\
	stock-icons/24/gtk-index.png			\
	stock-icons/24/gtk-orientation-landscape.png	\
	stock-icons/24/gtk-orientation-portrait.png	\
	stock-icons/24/gtk-orientation-reverse-landscape.png \
	stock-icons/24/gtk-orientation-reverse-portrait.png \
	stock-icons/24/gtk-page-setup.png		\
	stock-icons/24/gtk-preferences.png		\
	stock-icons/24/gtk-select-color.png		\
	stock-icons/24/gtk-select-font.png		\
	stock-icons/24/gtk-undelete-ltr.png		\
	stock-icons/24/gtk-undelete-rtl.png		\
	stock-icons/24/help-about.png			\
	stock-icons/24/help-contents.png		\
	stock-icons/24/image-missing.png		\
	stock-icons/24/list-add.png			\
	stock-icons/24/list-remove.png			\
	stock-icons/24/media-floppy.png			\
	stock-icons/24/media-optical.png		\
	stock-icons/24/media-playback-pause.png		\
	stock-icons/24/media-playback-start-ltr.png	\
	stock-icons/24/media-playback-start-rtl.png	\
	stock-icons/24/media-playback-stop.png		\
	stock-icons/24/media-record.png			\
	stock-icons/24/media-seek-backward-ltr.png	\
	stock-icons/24/media-seek-forward-ltr.png	\
	stock-icons/24/media-skip-backward-ltr.png	\
	stock-icons/24/media-skip-forward-ltr.png	\
	stock-icons/24/network-idle.png			\
	stock-icons/24/printer-error.png		\
	stock-icons/24/printer-info.png			\
	stock-icons/24/printer-paused.png		\
	stock-icons/24/printer-warning.png		\
	stock-icons/24/process-stop.png			\
	stock-icons/24/system-run.png			\
	stock-icons/24/text-x-generic.png		\
	stock-icons/24/tools-check-spelling.png		\
	stock-icons/24/view-fullscreen.png		\
	stock-icons/24/view-refresh.png			\
	stock-icons/24/view-restore.png			\
	stock-icons/24/view-sort-ascending.png		\
	stock-icons/24/view-sort-descending.png		\
	stock-icons/24/window-close.png			\
	stock-icons/24/zoom-fit-best.png		\
	stock-icons/24/zoom-in.png			\
	stock-icons/24/zoom-original.png		\
	stock-icons/24/zoom-out.png			\
	\
	stock-icons/32/gtk-dnd-multiple.png		\
	stock-icons/32/gtk-dnd.png			\
	\
	stock-icons/48/dialog-error.png			\
	stock-icons/48/dialog-information.png		\
	stock-icons/48/dialog-password.png		\
	stock-icons/48/dialog-question.png		\
	stock-icons/48/dialog-warning.png

GENERATED_ICONS = \
	stock-icons/16/go-previous-rtl.png 		\
	stock-icons/16/go-next-rtl.png 			\
	stock-icons/16/go-first-rtl.png 		\
	stock-icons/16/go-last-rtl.png 			\
	stock-icons/16/media-seek-forward-rtl.png 	\
	stock-icons/16/media-skip-forward-rtl.png 	\
	stock-icons/16/media-skip-backward-rtl.png 	\
	stock-icons/16/media-seek-backward-rtl.png 	\
	stock-icons/16/document-save.png 		\
	stock-icons/16/folder-remote.png 		\
	stock-icons/16/user-home.png 			\
	stock-icons/16/user-desktop.png 		\
	stock-icons/24/go-previous-rtl.png 		\
	stock-icons/24/go-next-rtl.png 			\
	stock-icons/24/go-first-rtl.png 		\
	stock-icons/24/go-last-rtl.png 			\
	stock-icons/24/media-seek-forward-rtl.png 	\
	stock-icons/24/media-skip-forward-rtl.png 	\
	stock-icons/24/media-skip-backward-rtl.png 	\
	stock-icons/24/media-seek-backward-rtl.png 	\
	stock-icons/24/document-save.png 		\
	stock-icons/24/folder-remote.png 		\
	stock-icons/24/user-home.png 			\
	stock-icons/24/user-desktop.png

# need to copy the icons so that we can replace the generated ones with
# symlinks, even in the readonly srcdir case
stamp-icons: $(STOCK_ICONS)
	if [ ! -d stock-icons ]; then 					\
	  for d in 16 20 24 32 48; do 					\
	    mkdir -p stock-icons/$$d; 					\
	  done;								\
	  for f in $(STOCK_ICONS); do					\
	    cp $(srcdir)/$$f $$(dirname $$f);				\
	  done;								\
	  chmod -R u+w stock-icons;					\
        fi &&								\
	for i in 16 24; do						\
	   (cd stock-icons/$$i						\
	   && $(RM) document-save.png 					\
	   && $(LN_S) media-floppy.png document-save.png 		\
	   && $(RM) folder-remote.png 					\
	   && $(LN_S) folder.png folder-remote.png 			\
	   && $(RM) go-first-rtl.png 					\
	   && $(LN_S) go-last-ltr.png go-first-rtl.png 			\
	   && $(RM) go-last-rtl.png 					\
	   && $(LN_S) go-first-ltr.png go-last-rtl.png 			\
	   && $(RM) go-next-rtl.png 					\
	   && $(LN_S) go-previous-ltr.png go-next-rtl.png 		\
	   && $(RM) go-previous-rtl.png 				\
	   && $(LN_S) go-next-ltr.png go-previous-rtl.png 		\
	   && $(RM) media-seek-backward-rtl.png 			\
	   && $(LN_S) media-seek-forward-ltr.png media-seek-backward-rtl.png \
	   && $(RM) media-seek-forward-rtl.png 				\
	   && $(LN_S) media-seek-backward-ltr.png media-seek-forward-rtl.png \
	   && $(RM) media-skip-backward-rtl.png 			\
	   && $(LN_S) media-skip-forward-ltr.png media-skip-backward-rtl.png \
	   && $(RM) media-skip-forward-rtl.png 				\
	   && $(LN_S) media-skip-backward-ltr.png media-skip-forward-rtl.png \
	   && $(RM) user-desktop.png 					\
	   && $(LN_S) folder.png user-desktop.png 			\
	   && $(RM) user-home.png 					\
	   && $(LN_S) folder.png user-home.png 				\
	) done 								\
	&& touch stamp-icons

gtkbuiltincache.h: @REBUILD@ stamp-icons


EXTRA_DIST +=                   \
	$(STOCK_ICONS)		\
	$(GENERATED_ICONS)	\
	line-arrow.xbm 		\
	line-wrap.xbm 		\
	tree_plus.xbm		\
	tree_minus.xbm          \
	tree_minus.xpm		\
	tree_plus.xpm		\
	gtkrc.default		\
	gtkrc.key.default	\
	gtkrc.key.emacs		\
	gtkrc.key.mac		\
	gtktypebuiltins.c.template      \
	gtktypebuiltins.h.template


install-data-local:

-include $(top_srcdir)/git.mk
