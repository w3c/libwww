## Process this file with Automake to create Makefile.in.

LDFLAGS = -version-info "1:0:1"

CFLAGS = @CFLAGS@ -DXML_NS

lib_LTLIBRARIES = \
	@XMLPARSE@

EXTRA_LTLIBRARIES = \
	libxmlparse.la

INCLUDES = -I$(srcdir)/../xmltok

libxmlparse_la_SOURCES = \
	hashtable.h \
	hashtable.c \
	xmlparse.c

libxmlparse_la_LDFLAGS = -rpath $(libdir)

pkginclude_HEADERS = \
	xmlparse.h
