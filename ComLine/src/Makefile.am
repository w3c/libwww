## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = w3c

w3c_LDADD = ../../Library/src/libwww.a -lm

INCLUDES = -I$(srcdir)/../../Library/src @X_CFLAGS@

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

#	Some extra make directives
inc : $(HEADERS)
	@echo Include files generated from hypertext.

all-local : inc

w3c_SOURCES = \
	HTLine.c

noinst_HEADERS = \
	HTLine.h
