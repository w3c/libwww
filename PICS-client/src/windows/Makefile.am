## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak *.dsp)
PL  := $(wildcard *.pl)

AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
	pics.def

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(SRC) \
	$(MAK) \
	$(PL)

def:  maintainer-clean-generic $(BUILT_SOURCES)

.files.def:
	$(top_srcdir)/config/makedefs.pl -headerEXPORTS -d$(srcdir)/.. @$(srcdir)/$(*F).files > $(srcdir)/$(*F).def

picslist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ../../../config/makedefs.pl @ > picslist.txt
