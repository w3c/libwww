## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = www

www_LDADD = \
	../../Library/src/libwww.la ../../PICS-client/src/libpics.a -lm

www_DEPENDENCIES = \
	../../Library/src/libwww.la ../../PICS-client/src/libpics.a

SUBDIRS = windows vms

INCLUDES = -I$(srcdir)/../../Library/src -I$(srcdir)/../../PICS-client/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
    ifdef WWW
	$(WWW) -w90 -na -p -to text/x-c $(srcdir)/$(*F).html -o $(srcdir)/$(*F).h
    else
	@echo "**************************************************"
	@echo "Need to regenerate one or more .h files but can't find the libwww"
	@echo "Line Mode Browser (www). Please run the following command:"
	@echo ""
	@echo 
	@echo "	perl $(top_srcdir)/config/winConfigure.pl $(srcdir)"
	@echo ""
	@echo "to generate the .h files and then retry make. Don't mind"
	@echo "any warning messages coming from the winConfigure.pl script."
	@echo "**************************************************"
	@exit 1
    endif

www_SOURCES = \
	HTBrowse.c	GridText.c	ConView.c \
	GridStyle.c	DefaultStyles.c

BUILT_SOURCES = \
	HTBrowse.h	GridStyle.h	GridText.h \
	HTFont.h

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
	DefaultStyles.c.modern \
	a_stdio.g \
	a_stdio.h
