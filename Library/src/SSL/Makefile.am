## Process this file with Automake to create Makefile.in.

AM_LDFLAGS = -version-info "1:0:1"

lib_LTLIBRARIES = @WWWSSL@

EXTRA_LTLIBRARIES = \
	libwwwssl.la

SUBDIRS = windows

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html .txt
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

libwwwssl_la_SOURCES = \
	WWWSSL.h \
	HTSSL.h \
	HTSSLMan.h \
	HTSSL.c \
	HTSSLReader.h \
	HTSSLReader.c \
	HTSSLWriter.h \
	HTSSLWriter.c \
	HTSSLhttps.h \
	HTSSLhttps.c

libwwwssl_la_LDFLAGS = -rpath $(libdir)

LDADD = \
	libwwwssl.la

libwwwssl_la_CPPFLAGS = \
	-I/usr/local/include \
	-I${top_srcdir}/modules/md5 \
	-I${top_srcdir}/modules/expat/xmlparse \
	-I$(top_srcdir)/Library/src \
	@SSLINC@

BUILT_SOURCES = \
	WWWSSL.h \
	HTSSL.h \
	HTSSLMan.h \
	HTSSLReader.h \
	HTSSLWriter.h \
	HTSSLhttps.h

pkginclude_HEADERS = $(BUILT_SOURCES)

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS)
