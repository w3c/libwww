## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = www

www_LDADD = \
	../../Library/src/libwww.a ../../PICS-client/src/libpics.a -lm

www_DEPENDENCIES = \
	../../Library/src/libwww.a ../../PICS-client/src/libpics.a

#	For easier use - NOT THE DEFAULT!
AUTOMAKE_OPTIONS = foreign

SUBDIRS = windows vms

INCLUDES = -I$(srcdir)/../../Library/src -I$(srcdir)/../../PICS-client/src -I$(srcdir)/@WAISINCLUDE@

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

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
	purify -cache-dir=$(PURIFY_CACHE) \
	$(LINK) $(www_OBJECTS) $(www_LDADD) $(LIBS)

# need the WAIS library and the cyrrilic support!

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	DefaultStyles.c.classic \
	DefaultStyles.c.modern \
	a_stdio.g \
	a_stdio.h
