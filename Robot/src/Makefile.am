## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = webbot

webbot_LDADD = ../../Library/src/libwww.a -lm

webbot_DEPENDENCIES = \
	../../Library/src/libwww.a

SUBDIRS = windows

INCLUDES = -I$(srcdir)/../../Library/src

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

webbot_SOURCES = \
	HTRobot.c

BUILT_SOURCES = \
	HTRobot.h

EXTRA_DIST = \
	 $(BUILT_SOURCES) \
	Robot.tcl \
	robot.sh

inc: $(BUILT_SOURCES)
	@echo Generated built sources

PURIFY_CACHE = @PURIFY_CACHE@

purify: $(webbot_OBJECTS) $(webbot_DEPENDENCIES)
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(webbot_OBJECTS) $(webbot_LDADD) $(LIBS)
