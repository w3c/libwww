## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

SUBDIRS = windows

lib_LIBRARIES = libpics.a

SUBDIRS = windows

INCLUDES = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
    ifdef WWW
	$(WWW) -w90 -na -p -to text/x-c $(srcdir)/$(*F).html -o $(srcdir)/$(*F).h
    else
	@echo "**************************************************"
	@echo "Need to regenerate one or more .h files but can't find the libwww"
	@echo "Line Mode Browser (www). Attempting to generate them using the"
	@echo "winConfigure.pl script - it may generate a few warnings."
	@echo "**************************************************"
	perl $(top_srcdir)/config/winConfigure.pl $(srcdir)
    endif

inc: $(BUILT_SOURCES)
	@echo Generated built sources

libpics_a_SOURCES = \
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
