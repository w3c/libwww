## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = head libapp_1 libapp_2 libapp_3 libapp_4 init \
	chunk chunkbody LoadToFile postform multichunk put post trace \
	range tzcheck mget isredirected listen eventloop memput \
	showlinks showtags showtext tiny showxml upgrade

LDADD = \
	-lm \
	../src/libwww.la \
	../../modules/md5/libmd5.la \
	../../modules/expat/xmlparse/libxmlparse.la \
	../../modules/expat/xmltok/libxmltok.la

INCLUDES = \
	-I$(srcdir)/../src \
	-I$(top_srcdir)/modules/expat/xmlparse

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(DOCS)

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(tiny_OBJECTS) $(tiny_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(tiny_OBJECTS) \
	../src/.libs/libwww.a -lm $(LIBS)
