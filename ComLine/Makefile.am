## Process this file with Automake to create Makefile.in.

SUBDIRS = src User

BUILT_SOURCES = \
	README

EXTRA_DIST = \
	$(BUILT_SOURCES)

$(srcdir)/README : $(srcdir)/User/CommandLine.html
	www -p -na $(srcdir)/User/CommandLine.html \
		-o $(srcdir)/README
