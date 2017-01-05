## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = www

www_LDADD = \
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
	$(top_builddir)/PICS-client/src/libpics.la \
	-lm @LIBWWWXML@ @LIBWWWZIP@ @LIBWWWWAIS@ @LIBWWWSQL@ @LIBWWWMD5@ @A_STDIO@

SUBDIRS = windows vms

www_CPPFLAGS = \
	-I$(srcdir)/../../Library/src \
	-I$(top_srcdir)/modules/expat/lib \
	-I$(srcdir)/../../PICS-client/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .g .h .html
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

.g.c:
	-$(RM) $(srcdir)/$(*F).c
	$(LN_S) $(srcdir)/$(*F).g $(srcdir)/$(*F).c

www_SOURCES = \
	HTBrowse.c	GridText.c	ConView.c \
	GridStyle.c	DefaultStyles.c

BUILT_SOURCES = \
	HTBrowse.h	GridStyle.h	GridText.h \
	HTFont.h	a_stdio.c

EXTRA_www_SOURCES = \
	a_stdio.g \
	a_stdio.h

DOCS :=	$(wildcard *.html)

inc: $(BUILT_SOURCES)
	@echo Generated built sources

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(www_OBJECTS) $(www_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(www_OBJECTS) \
	../../Library/src/.libs/libwww.a -lm $(LIBS)

# need the WAIS library and the cyrrilic support!

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS) \
	DefaultStyles.c.classic \
	DefaultStyles.c.modern
