## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = webbot

webbot_LDADD = ../../Library/src/libwww.la -lm

webbot_DEPENDENCIES = \
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
	-rm purify; \
	purify -cache-dir=$(PURIFY_CACHE) \
	$(CC) $(LDFLAGS) -o purify $(webbot_OBJECTS) \
	../../Library/src/.libs/libwww.a -lm $(LIBS)
