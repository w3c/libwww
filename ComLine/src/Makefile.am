## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = w3c

w3c_LDADD = ../../Library/src/libwww.la -lm

w3c_DEPENDENCIES = \
	../../Library/src/libwww.la

SUBDIRS = windows

INCLUDES = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
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

w3c_SOURCES = \
	HTLine.c

BUILT_SOURCES = \
	HTLine.h

EXTRA_DIST = \
	$(BUILT_SOURCES)

inc: $(BUILT_SOURCES)
	@echo Generated built sources

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(w3c_OBJECTS) $(w3c_DEPENDENCIES)
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(w3c_OBJECTS) \
	../../Library/src/.libs/libwww.a -lm $(LIBS)

