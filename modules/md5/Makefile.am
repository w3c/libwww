## Process this file with Automake to create Makefile.in.

lib_LTLIBRARIES = libmd5.la
libmd5_la_LDFLAGS = -version-info "1:0:1"

libmd5_la_SOURCES = \
	md5.c \
	md5.h

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(DOCS)
