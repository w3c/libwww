# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 0.31 from Makefile.am

# Copyright (C) 1994, 1995, 1996 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../../..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

pkgdatadir = $(datadir)/w3c-libwww
pkglibdir = $(libdir)/w3c-libwww
pkgincludedir = $(includedir)/w3c-libwww

top_builddir = ../../..

INSTALL = /usr/local/bin/ginstall -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL} -m 755
transform = s,x,x,

SUFFIXES = .files .def

BUILT_SOURCES = \
	wwwapp.def	wwwapp.def	wwwlist.txt \
	wwwcore.def	wwwdir.def	wwwdll.def \
	wwwfile.def	wwwftp.def	wwwgophe.def \
	wwwguess.def	wwwhtml.def	wwwhttp.def \
	wwwmime.def	wwwnews.def	wwwrules.def \
	wwwtelnt.def	wwwutils.def	wwwwais.def

EXTRA_DIST = \
	wwwdll.def \
	$(SRC) \
	$(MAK) \
	$(PL)
CONFIG_HEADER = ../../../config.h
DIST_COMMON = README Makefile.am Makefile.in README


PACKAGE = w3c-libwww
VERSION = 4.1

DISTFILES = $(DIST_COMMON) $(SOURCES) $(BUILT_SOURCES) $(HEADERS) \
	$(TEXINFOS) $(INFOS) $(MANS) $(EXTRA_DIST) $(DATA)
DEP_DISTFILES = $(DIST_COMMON) $(SOURCES) $(BUILT_SOURCES) $(HEADERS) \
	$(TEXINFOS) $(INFO_DEPS) $(MANS) $(EXTRA_DIST) $(DATA)
default: all


$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in
	cd $(top_srcdir) && automake $(subdir)/Makefile

Makefile: $(top_builddir)/config.status Makefile.in
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= ./config.status
tags: TAGS
TAGS:


subdir = Library/src/windows
distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)
dist: $(DEP_DISTFILES)
	@for file in `cd $(srcdir) && echo $(DISTFILES)`; do \
	  test -f $(distdir)/$$file \
	  || ln $(srcdir)/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $(srcdir)/$$file $(distdir)/$$file; \
	done
info:

dvi:

check: all

installcheck:

install-exec: 

install-data: 

install: install-exec install-data
	@:

uninstall: 

all: Makefile

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install
installdirs:


mostlyclean-generic:
	test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	rm -f Makefile $(DISTCLEANFILES)
	rm -f config.cache config.log $(CONFIG_HEADER) stamp-h

maintainer-clean-generic:
	test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean 

distclean:  distclean-generic clean 
	rm -f config.status

maintainer-clean:  maintainer-clean-generic distclean 
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default tags dist info dvi check installcheck install-exec \
install-data install uninstall all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
PL  := $(wildcard *.pl)

.files.def:
	./makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def

wwwlist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ./makedefs.pl @ > wwwlist.txt
.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
