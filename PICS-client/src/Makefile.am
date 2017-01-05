## Process this file with Automake to create Makefile.in.

lib_LTLIBRARIES = libpics.la

SUBDIRS = windows

libpics_la_CPPFLAGS = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
    ifdef WWW
	$(WWW) -w90 -na -p -to text/x-c $(srcdir)/$(*F).html -o $(srcdir)/$(*F).h
    else
	@echo "**************************************************"
	@echo "Need to regenerate one or more .h files."
	@echo "Attempting to generate them using the"
	@echo "winConfigure.pl script - it may generate a few warnings."
	@echo "**************************************************"
	perl $(top_srcdir)/config/winConfigure.pl $(srcdir)
    endif

inc: $(BUILT_SOURCES)
	@echo Generated built sources

libpics_la_SOURCES = \
	CSChkLab.c	CSStream.c	CSKwik.c \
	CSLabel.c	CSLLOut.c	CSLLURLs.c \
	CSMacRed.c	CSMem.c		CSApp.c \
	CSParse.c	CSUsrLst.c	CSUser.c

BUILT_SOURCES = \
	CSLApp.h	CSLUtils.h \
	CSLL.h		CSLLSt.h \
	CSMR.h		CSMRSt.h \
	CSUser.h	CSUserSt.h \
	CSParse.h	CSUsrLst.h

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS)
