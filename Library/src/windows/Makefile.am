## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
FLS := $(wildcard *.files)

BUILT_SOURCES = \
	wwwapp.def	wwwapp.def	wwwcache.def \
	wwwcore.def	wwwdir.def 	wwwmux.def \
	wwwfile.def	wwwftp.def	wwwgophe.def \
	wwwstream.def	wwwhtml.def	wwwhttp.def \
	wwwmime.def	wwwnews.def 	wwwtrans.def \
	wwwtelnt.def	wwwutils.def	wwwwais.def \
	wwwzip.def	wwwsql.def	wwwxml.def \
	wwwinit.def

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	config.h \
	wwwdll.def \
	$(SRC) \
	$(FLS) \
	$(MAK)

def-clean:
	test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)

def: $(BUILT_SOURCES)

.files.def:
	perl $(top_srcdir)/config/makedefs.pl -headerEXPORTS -d$(srcdir)/.. @$(srcdir)/$(*F).files > $(srcdir)/$(*F).def



