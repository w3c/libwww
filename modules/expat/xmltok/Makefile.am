## Process this file with Automake to create Makefile.in.

LDFLAGS = -version-info "1:0:1"

lib_LTLIBRARIES = \
	@XMLTOK@

EXTRA_LTLIBRARIES = \
	libxmltok.la

libxmltok_la_SOURCES = \
	asciitab.h \
	iasciitab.h \
	nametab.h \
	xmldef.h \
	xmlrole.h \
	xmltok.h \
	xmltok_impl.h \
	latin1tab.h \
	utf8tab.h \
	xmlrole.c \
	xmltok.c

libxmltok_la_LDFLAGS = -rpath $(libdir)

EXTRA_DIST = \
	xmltok_impl.c
