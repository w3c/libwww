## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = w3c

w3c_LDADD = \
	$(top_builddir)/Library/src/libwwwinit.la \
	$(top_builddir)/Library/src/libwwwapp.la \
	$(top_builddir)/Library/src/libwwwhtml.la \
	$(top_builddir)/Library/src/libwwwtelnet.la \
	$(top_builddir)/Library/src/libwwwnews.la \
	$(top_builddir)/Library/src/libwwwhttp.la \
	$(top_builddir)/Library/src/libwwwmime.la \
	$(top_builddir)/Library/src/libwwwgopher.la \
	$(top_builddir)/Library/src/libwwwftp.la \
	$(top_builddir)/Library/src/libwwwfile.la \
	$(top_builddir)/Library/src/libwwwdir.la \
	$(top_builddir)/Library/src/libwwwcache.la \
	$(top_builddir)/Library/src/libwwwstream.la \
	$(top_builddir)/Library/src/libwwwmux.la \
	$(top_builddir)/Library/src/libwwwtrans.la \
	$(top_builddir)/Library/src/libwwwcore.la \
	$(top_builddir)/Library/src/libwwwutils.la \
	-lm @LIBWWWXML@ @LIBWWWZIP@ @LIBWWWWAIS@ @LIBWWWSQL@ @LIBWWWMD5@

SUBDIRS = windows

w3c_CPPFLAGS = \
	-I$(srcdir)/../../Library/src \
	-I$(top_srcdir)/modules/expat/lib

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

w3c_SOURCES = \
	HTLine.c

BUILT_SOURCES = \
	HTLine.h

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(DOCS) \
	$(BUILT_SOURCES)

inc: $(BUILT_SOURCES)
	@echo Generated built sources

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(w3c_OBJECTS) $(w3c_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(w3c_OBJECTS) \
	-lm \
	../../Library/src/.libs/libwwwinit.a \
	../../Library/src/.libs/libwwwapp.a \
	../../Library/src/.libs/libwwwxml.a \
	../../Library/src/.libs/libwwwhtml.a \
	../../Library/src/.libs/libwwwtelnet.a \
	../../Library/src/.libs/libwwwnews.a \
	../../Library/src/.libs/libwwwhttp.a \
	../../Library/src/.libs/libwwwmime.a \
	../../Library/src/.libs/libwwwgopher.a \
	../../Library/src/.libs/libwwwftp.a \
	../../Library/src/.libs/libwwwdir.a \
	../../Library/src/.libs/libwwwcache.a \
	../../Library/src/.libs/libwwwstream.a \
	../../Library/src/.libs/libwwwfile.a \
	../../Library/src/.libs/libwwwmux.a \
	../../Library/src/.libs/libwwwtrans.a \
	../../Library/src/.libs/libwwwcore.a \
	../../Library/src/.libs/libwwwutils.a \
	../../Library/src/.libs/libwwwzip.a \
	../../modules/md5/.libs/libmd5.a \
	../../modules/expat/xmlparse/.libs/libxmlparse.a \
	../../modules/expat/xmltok/.libs/libxmltok.a \
	$(LIBS)

