## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

DOCS :=	$(wildcard *.html *.gif)

EXTRA_DIST = \
	$(DOCS)

SUBDIRS = Patch Architecture Using Guide Style Platform

BUILT_SOURCES = \
	Extrnals.html

Extrnals.html:
	../../config/makeindx.pl ../src ../src/windows/*.docs >Extrnals.html

