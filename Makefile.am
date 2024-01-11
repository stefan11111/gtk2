## Makefile.am for GTK+
include $(top_srcdir)/Makefile.decl

SRC_SUBDIRS = gdk gtk modules   perf
SUBDIRS = $(SRC_SUBDIRS) m4macros build

# require automake 1.4
AUTOMAKE_OPTIONS = 1.7
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST +=			\
	HACKING			\
	makecopyright		\
	NEWS.pre-1-0		\
	ChangeLog.pre-1-0	\
	ChangeLog.pre-1-2	\
	ChangeLog.pre-2-0	\
	ChangeLog.pre-2-2	\
	ChangeLog.pre-2-4	\
	ChangeLog.pre-2-6	\
	ChangeLog.pre-2-8	\
	ChangeLog.pre-2-10	\
	ChangeLog.pre-2-12	\
	ChangeLog.pre-2-14	\
	ChangeLog.pre-2-16	\
	ChangeLog.gtk-async-file-chooser	\
	ChangeLog.gtk-printing	\
	README.commits	\
	README.win32		\
	config.h.win32		\
	makefile.msc	\
	gtk-zip.sh.in		\
	sanitize-la.sh

MAINTAINERCLEANFILES = \
	$(srcdir)/INSTALL \
	$(srcdir)/README \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/autoscan.log \
	$(srcdir)/compile \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in \
	$(srcdir)/config.sub \
	$(srcdir)/configure.scan \
	$(srcdir)/depcomp \
	$(srcdir)/install-sh \
	$(srcdir)/ltmain.sh \
	$(srcdir)/missing \
	$(srcdir)/mkinstalldirs \
	$(srcdir)/omf.make \
	$(srcdir)/ChangeLog \
	`find "$(srcdir)" -type f -name Makefile.in -print`

GDKTARGET=@gdktarget@

## Copy .pc files to target-specific names
gtk+-$(GDKTARGET)-2.0.pc: gtk+-2.0.pc
	rm -f gtk+-$(GDKTARGET)-2.0.pc && \
	cp gtk+-2.0.pc gtk+-$(GDKTARGET)-2.0.pc

gdk-$(GDKTARGET)-2.0.pc: gdk-2.0.pc
	rm -f gdk-$(GDKTARGET)-2.0.pc && \
	cp gdk-2.0.pc gdk-$(GDKTARGET)-2.0.pc

gtk+-$(GDKTARGET)-2.0-uninstalled.pc: gtk+-2.0-uninstalled.pc
	rm -f gtk+-$(GDKTARGET)-2.0-uninstalled.pc && \
	cp gtk+-2.0-uninstalled.pc gtk+-$(GDKTARGET)-2.0-uninstalled.pc

gdk-$(GDKTARGET)-2.0-uninstalled.pc: gdk-2.0-uninstalled.pc
	rm -f gdk-$(GDKTARGET)-2.0-uninstalled.pc && \
	cp gdk-2.0-uninstalled.pc gdk-$(GDKTARGET)-2.0-uninstalled.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA= gdk-$(GDKTARGET)-2.0.pc gtk+-$(GDKTARGET)-2.0.pc

if OS_UNIX
pkgconfig_DATA += gtk+-unix-print-2.0.pc
endif

DISTCLEANFILES =				\
	gtk+-unix-print-2.0.pc			\
	gtk+-$(GDKTARGET)-2.0.pc		\
	gdk-$(GDKTARGET)-2.0.pc			\
	gtk+-$(GDKTARGET)-2.0-uninstalled.pc	\
	gdk-$(GDKTARGET)-2.0-uninstalled.pc	\
	config.lt

distclean-local:
	if test "$(srcdir)" = "."; then :; else \
		rm -f ChangeLog; \
	fi

ChangeLog:
	@echo Creating $@
	@if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git ./missing --run git log GTK_2_16_0^^.. --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi

## copy the default target for this platform to gdk-2.0.pc and gtk+-2.0.pc
DEFAULT_GDKTARGET=x11
install-data-hook:
	(cd $(DESTDIR)$(pkgconfigdir) && \
	 test -f gdk-$(DEFAULT_GDKTARGET)-2.0.pc && \
	 test -f gtk+-$(DEFAULT_GDKTARGET)-2.0.pc && \
	 rm -f gdk-2.0.pc && cp -f gdk-$(DEFAULT_GDKTARGET)-2.0.pc gdk-2.0.pc && \
	 rm -f gtk+-2.0.pc && cp -f gtk+-$(DEFAULT_GDKTARGET)-2.0.pc gtk+-2.0.pc) || \
	(cd $(DESTDIR)$(pkgconfigdir) && \
	 rm -f gdk-2.0.pc && cp -f gdk-$(GDKTARGET)-2.0.pc gdk-2.0.pc && \
	 rm -f gtk+-2.0.pc && cp -f gtk+-$(GDKTARGET)-2.0.pc gtk+-2.0.pc)

uninstall-local:
	rm -f $(DESTDIR)$(pkgconfigdir)/gdk-2.0.pc
	rm -f $(DESTDIR)$(pkgconfigdir)/gtk+-2.0.pc

dist-hook:
	if test -f $(srcdir)/INSTALL.in && test -f $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in" \
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi

.PHONY: files release sanity snapshot ChangeLog

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
	@for subdir in $(SUBDIRS); do \
	  files=`cd $$subdir; $(MAKE) files | grep -v "make\[[1-9]\]"`; \
	  for file in $$files; do \
	    echo $$subdir/$$file; \
	  done; \
	done

release:
	rm -rf .deps */.deps
	$(MAKE) distcheck

sanity:
	./sanity_check $(VERSION)


snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-snap`date +"%Y%m%d"`

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-man --disable-rebuilds --enable-introspection

-include $(top_srcdir)/git.mk
