## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = webbot

webbot_LDADD = \
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
	-lm @LIBWWWXML@ @LIBWWWZIP@ @LIBWWWWAIS@ \
	@LIBWWWSQL@ @LIBWWWMD5@ @LIBWWWSSL@ \
	$(MYSQL_LIBS)

SUBDIRS = windows

webbot_CPPFLAGS = \
	-I$(srcdir)/../../Library/src \
	-I$(srcdir)/../../Library/src/SSL \
	-I$(top_srcdir)/modules/expat/lib \
	$(MYSQL_CFLAGS)

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

webbot_SOURCES = \
	HTRobot.c RobotMain.c RobotTxt.c HTQueue.c

BUILT_SOURCES = \
	HTRobot.h HTRobMan.h RobotTxt.h HTQueue.h

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS) \
	robot.sh

inc: $(BUILT_SOURCES)
	@echo Generated built sources

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(webbot_OBJECTS) $(webbot_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(webbot_OBJECTS) \
	-lm \
	../../Library/src/.libs/libwww.a \
	../../modules/md5/.libs/libmd5.a \
	../../modules/expat/.libs/libexpat.a
	$(LIBS)




