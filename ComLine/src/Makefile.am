## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = w3c

w3c_LDADD = ../../Library/src/libwww.a -lm

w3c_DEPENDENCIES = \
	../../Library/src/libwww.a

INCLUDES = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

w3c_SOURCES = \
	HTLine.c

BUILT_SOURCES = \
	HTLine.h
