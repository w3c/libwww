## Process this file with Automake to create Makefile.in.

all:
	@echo "**************************************************"
	@echo "The line mode browser is not currently compiled as"
	@echo "the changes to the HText interface hasn't been"
	@echo "integrated into the line mode browser code."
	@echo "Your help is greatly appreciated!"
	@echo "**************************************************"

EXTRA_PROGRAMS = www		# Should be www_PROGRAMS

www_LDADD = \
	../../Library/src/libwww.la \
	../../PICS-client/src/libpics.a \
	../../modules/md5/libmd5.la \
	../../modules/expat/xmltok/libxmltok.la \
	../../modules/expat/xmlparse/libxmlparse.la \
	-lm @A_STDIO@

SUBDIRS = windows vms

INCLUDES = \
	-I$(srcdir)/../../Library/src \
	-I$(top_srcdir)/modules/expat/xmlparse \
	-I$(srcdir)/../../PICS-client/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .g .h .html
.html.h:
    ifdef WWW
	$(WWW) -w90 -na -p -to text/x-c $(srcdir)/$(*F).html -o $(srcdir)/$(*F).h
    else
	@echo "**************************************************"
	@echo "Need to regenerate one or more .h files but can't find the libwww"
	@echo "Line Mode Browser (www). Attempting to generate them using the"
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
	DefaultStyles.c.classic \
	DefaultStyles.c.modern
