## Process this file with Automake to create Makefile.in.

noinst_PROGRAMS = @CVS2SQL@
EXTRA_PROGRAMS = cvs2sql

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
	../src/libwwwfile.la \
	../src/libwwwdir.la \
	../src/libwwwcache.la \
	../src/libwwwstream.la \
	../src/libwwwmux.la \
	../src/libwwwtrans.la \
	../src/libwwwcore.la \
	../src/libwwwutils.la \
	-lm @LIBWWWZIP@ @LIBWWWWAIS@ @LIBWWWSQL@ @LIBWWWMD5@ \
	$(MYSQL_LIBS)

cvs2sql_CPPFLAGS = \
	-I$(srcdir)/../../Library/src \
	$(MYSQL_CFLAGS)
