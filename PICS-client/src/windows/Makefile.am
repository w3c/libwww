## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
PL  := $(wildcard *.pl)

AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
	pics.def	picslist.txt

EXTRA_DIST = \
	$(SRC) \
	$(MAK) \
	$(PL)

.files.def:
	../../../config/makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def

picslist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ../../../config/makedefs.pl @ > picslist.txt


