## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
FLS := $(wildcard *.files)

BUILT_SOURCES = \
	wwwssl.def

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	wwwdll.def \
	$(SRC) \
	$(FLS) \
	$(MAK)

def-clean:
	test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)

def: $(BUILT_SOURCES)

.files.def:
	perl $(top_srcdir)/config/makedefs.pl -headerEXPORTS -d$(srcdir)/.. @$(srcdir)/$(*F).files > $(srcdir)/$(*F).def



