## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
PL  := $(wildcard *.pl)

BUILT_SOURCES = \
	wwwapp.def	wwwapp.def	wwwcache.def \
	wwwcore.def	wwwdir.def	wwwdll.def \
	wwwfile.def	wwwftp.def	wwwgophe.def \
	wwwguess.def	wwwhtml.def	wwwhttp.def \
	wwmime.def	wwwnews.def	wwwrules.def \
	wwwtelnt.def	wwwutils.def	wwwwais.def \
	wwwlist.txt

EXTRA_DIST = \
	$(SRC) \
	$(MAK) \
	$(PL)

.files.def:
	./makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def

wwwlist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ./makedefs.pl @ > wwwlist.txt


