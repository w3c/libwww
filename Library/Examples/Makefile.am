## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = head libapp_1 libapp_2 libapp_3 libapp_4 init \
	chunk chunkbody LoadToFile postform multichunk put post trace \
	range tzcheck mget isredirected listen eventloop memput \
	showlinks showtags showtext tiny

LDADD = ../src/libwww.la -lm

DEPENDENCIES = \
	../src/libwww.la

INCLUDES = -I$(srcdir)/../src

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(tiny_OBJECTS) $(tiny_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(tiny_OBJECTS) \
	../src/.libs/libwww.a -lm $(LIBS)
