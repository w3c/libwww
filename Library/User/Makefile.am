## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

DOCS :=	$(wildcard *.html *.gif)

EXTRA_DIST = \
	$(DOCS)

SUBDIRS = Patch Architecture Using Guide Style Platform

BUILT_SOURCES = \
	Extrnals.html

Extrnals.html:
	$(top_srcdir)/config/makeindx.pl $(srcdir)/../src $(srcdir)/../src/WWW*.html > Extrnals.html
