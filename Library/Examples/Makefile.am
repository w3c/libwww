## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = head libapp_1 libapp_2 libapp_3 libapp_4 init chunk chunkbody LoadToFile postform multichunk put post

LDADD = ../../Library/src/libwww.la -lm

DEPENDENCIES = \
	../../Library/src/libwww.la

INCLUDES = -I$(srcdir)/../../Library/src

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(post_OBJECTS) $(post_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(post_OBJECTS) \
	../src/.libs/libwww.a -lm $(LIBS)
