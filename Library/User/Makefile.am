## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

DOCS :=	$(wildcard *.html *.gif)

EXTRA_DIST = \
	$(DOCS)

SUBDIRS = Patch Architecture Using Guide Style Platform

BUILT_SOURCES = \
	Extrnals.html

SUFFIXES = .html

DEPENDENCIES = \
	../src/libwww.la

Extrnals.html: $(DEPENDENCIES)
	-rm $(srcdir)/Extrnals.html;
	(cd $(srcdir) && \
	$(top_srcdir)/config/makeindx.pl ../src/ ../src/WWW*.html > $(srcdir)/Extrnals.html)
