## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = www

www_LDADD = ../../Library/src/libwww.a ../../PICS/src/libpics.a -lm

SUBDIRS = windows vms

INCLUDES = -I$(srcdir)/../../Library/src -I$(srcdir)/../../PICS/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

#	Some extra make directives
inc : $(HEADERS)
	@echo Include files generated from hypertext.

all-local : inc

www_SOURCES = \
	HTBrowse.c	GridText.c	ConView.c \
	GridStyle.c	DefaultStyles.c

noinst_HEADERS = \
	HTBrowse.h	GridStyle.h	GridText.h \
	HTFont.h

# need the WAIS library and the cyrrilic support!

DIST_OTHER = \
	DefaultStyles.c.classic \
	DefaultStyles.c.modern \
 	a_stdio.g \
	a_stdio.h
