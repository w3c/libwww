## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = webbot

webbot_LDADD = ../../Library/src/libwww.a -lm

webbot_DEPENDENCIES = \
	../../Library/src/libwww.a

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
