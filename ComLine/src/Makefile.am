## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = w3c

w3c_LDADD = ../../Library/src/libwww.a -lm

w3c_DEPENDENCIES = \
	../../Library/src/libwww.a

SUBDIRS = windows

INCLUDES = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

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
	purify -cache-dir=$(PURIFY_CACHE) \
	$(LINK) $(w3c_OBJECTS) $(w3c_LDADD) $(LIBS)

insure: $(w3c_OBJECTS) $(w3c_DEPENDENCIES)
	insure -static \
	$(LINK) $(w3c_OBJECTS) $(w3c_LDADD) $(LIBS)

