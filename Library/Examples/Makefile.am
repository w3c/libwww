## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = head libapp_1 libapp_2 libapp_3 libapp_4 init \
	chunk chunkbody LoadToFile postform multichunk put post trace \
	range tzcheck mget isredirected listen eventloop memput \
	showlinks showtags showtext tiny upgrade cookie @SHOWXML@

EXTRA_PROGRAMS = showxml

LDADD = \
	../src/libwwwinit.la \
	../src/libwwwapp.la \
	@LIBWWWXML@ ../src/libwwwhtml.la \
	../src/libwwwtelnet.la \
	../src/libwwwnews.la \
	../src/libwwwhttp.la \
	../src/libwwwmime.la \
	../src/libwwwgopher.la \
	../src/libwwwftp.la \
	../src/libwwwdir.la \
	../src/libwwwcache.la \
	../src/libwwwstream.la \
	../src/libwwwfile.la \
	../src/libwwwmux.la \
	../src/libwwwtrans.la \
	../src/libwwwcore.la \
	../src/libwwwutils.la \
	-lm @LIBWWWZIP@ @LIBWWWWAIS@ @LIBWWWSQL@ @LIBWWWMD5@

INCLUDES = \
	-I$(srcdir)/../src \
	-I$(top_srcdir)/modules/expat/xmlparse

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(DOCS)

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(cookie_OBJECTS) $(cookie_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(cookie_OBJECTS) \
	-lm \
	../src/.libs/libwww.a \
	../../modules/md5/.libs/libmd5.a \
	../../modules/expat/xmlparse/.libs/libxmlparse.a \
	../../modules/expat/xmltok/.libs/libxmltok.a \
	$(LIBS)
