## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

noinst_PROGRAM = export

DOCS :=	$(wildcard *.html *.gif)

EXTRA_DIST = \
	$(DOCS)

SUBDIRS = Patch Architecture Using Guide Style Platform

BUILT_SOURCES = \
	Extrnals.html

SUFFIXES = .html

DEPENDENCIES = \
	../src/libwww.la

Extrnals.html:
	(cd $(srcdir) && \
	perl ../../config/makeindx.pl ../src/ ../src/WWW*.html \
		> Extrnals.html)

export: Extrnals.html
	-rm $(srcdir)/Extrnals.html;
	(cd $(srcdir) && \
	perl ../../config/makeindx.pl ../src/ ../src/WWW*.html \
		> Extrnals.html)
