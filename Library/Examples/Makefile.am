## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = head libapp_1 libapp_2 libapp_3 libapp_4 init \
	chunk chunkbody LoadToFile postform multichunk put post trace \
	range tzcheck mget isredirected listen eventloop memput \
	getheaders showlinks showtags showtext tiny upgrade cookie \
        @DAVSAMPLE@ @MYEXT@ @SHOWXML@ @WWWSSLEX@

EXTRA_PROGRAMS = myext myext2 davsample showxml ptri stri wwwssl rdf_parse_file rdf_parse_buffer

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
        @LIBWWWDAV@ \
	@LIBWWWSSL@ \
	-lm @LIBWWWZIP@ @LIBWWWWAIS@ @LIBWWWSQL@ @LIBWWWMD5@

AM_CPPFLAGS = \
	-I$(srcdir)/../src \
	-I$(top_srcdir)/modules/expat/lib \
	-I$(srcdir)/../src/SSL

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(DOCS)

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(mget_OBJECTS) $(mget_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(mget_OBJECTS) \
	-lm \
	../src/.libs/libwwwinit.a \
	../src/.libs/libwwwapp.a \
	../src/.libs/libwwwxml.a \
	../src/.libs/libwwwhtml.a \
	../src/.libs/libwwwtelnet.a \
	../src/.libs/libwwwnews.a \
	../src/.libs/libwwwhttp.a \
	../src/.libs/libwwwmime.a \
	../src/.libs/libwwwgopher.a \
	../src/.libs/libwwwftp.a \
	../src/.libs/libwwwdir.a \
	../src/.libs/libwwwcache.a \
	../src/.libs/libwwwstream.a \
	../src/.libs/libwwwfile.a \
	../src/.libs/libwwwmux.a \
	../src/.libs/libwwwtrans.a \
	../src/.libs/libwwwcore.a \
	../src/.libs/libwwwutils.a \
        ../src/.libs/libwwwdav.a \
	../src/.libs/libwwwzip.a \
	../../modules/md5/.libs/libmd5.a \
	../../modules/expat/.libs/libexpat.a
	$(LIBS)


