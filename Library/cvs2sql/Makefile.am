## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = @CVS2SQL@
EXTRA_PROGRAMS = cvs2sql

LDADD = \
	-lm \
	../src/libwww.la \
	../../modules/md5/libmd5.la \
	../../modules/expat/xmlparse/libxmlparse.la \
	../../modules/expat/xmltok/libxmltok.la


INCLUDES = \
	-I$(srcdir)/../../Library/src
