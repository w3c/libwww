## Process this file with Automake to create Makefile.in.

SUBDIRS = src User

BUILT_SOURCES = \
	README

EXTRA_DIST = \
	$(BUILT_SOURCES)

$(srcdir)/README : $(srcdir)/User/CommandLine.html
	-cp $(srcdir)/User/CommandLine.html $(srcdir)/README.html
	-www -p -na $(srcdir)/README.html \
		 -o $(srcdir)/README
