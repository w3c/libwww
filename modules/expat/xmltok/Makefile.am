## Process this file with Automake to create Makefile.in.

LDFLAGS = -version-info "1:0:1"

CFLAGS = @CFLAGS@ -DXML_NS

lib_LTLIBRARIES = \
	@XMLTOK@

EXTRA_LTLIBRARIES = \
	libxmltok.la

libxmltok_la_SOURCES = \
	ascii.h \
	asciitab.h \
	iasciitab.h \
	latin1tab.h \
	nametab.h \
	utf8tab.h \
	xmldef.h \
	xmlrole.h \
	xmltok.h \
	xmltok_impl.h \
	xmlrole.c \
	xmltok.c

libxmltok_la_LDFLAGS = -rpath $(libdir)

EXTRA_DIST = \
	xmltok_impl.c \
	xmltok_ns.c
