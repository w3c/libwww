## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = head libapp_1 libapp_2 libapp_3 libapp_4 init \
	chunk chunkbody LoadToFile postform multichunk put post trace \
	range tzcheck mget isredirected listen eventloop memput \
	showlinks showtags showtext

LDADD = ../../Library/src/libwww.la -lm

DEPENDENCIES = \
	../../Library/src/libwww.la

INCLUDES = -I$(srcdir)/../../Library/src
