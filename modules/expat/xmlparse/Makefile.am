## Process this file with Automake to create Makefile.in.

lib_LTLIBRARIES = libxmlparse.la
libxmlparse_la_LDFLAGS = -version-info "1:0:1"

INCLUDES = -I$(srcdir)/../xmltok

libxmlparse_la_SOURCES = \
	hashtable.h \
	hashtable.c \
	xmlparse.c

include_HEADERS = \
	xmlparse.h
