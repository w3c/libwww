## Process this file with Automake to create Makefile.in.

VMS = $(srcdir)/vms
WIN = $(srcdir)/windows
FTP = /afs/w3.org/devel/Dist/$(PROD)

bin_PROGRAMS = w3c

w3c_LDADD = ../../Library/src/libwww.a -lm

INCLUDES = -I$(srcdir)/../../Library/src @X_CFLAGS@

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

#	Some extra make directives
inc : $(HEADERS)
	@echo Include files generated from hypertext.

all-local : inc

w3c_SOURCES = \
	HTLine.c

noinst_HEADERS = \
	HTLine.h

# need the WAIS library and the cyrrilic support!

DIST_OTHER = \
	$(WIN)/*.rc \
	$(WIN)/*.ico \
	$(WIN)/*.mak \
	$(WIN)/*.def \
	$(WIN)/*.c \
	$(WIN)/*.h \
	$(WIN)/*.txt
