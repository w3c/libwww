## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

SUBDIRS = windows

lib_LIBRARIES = pics

SUBDIRS = windows

INCLUDES = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

inc: $(BUILT_SOURCES)
	@echo Generated built sources

pics_SOURCES = \
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

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS)
