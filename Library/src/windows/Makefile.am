## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
PL  := $(wildcard *.pl)

BUILT_SOURCES = \
	wwwapp.def	wwwapp.def	wwwlist.txt \
	wwwcore.def	wwwdir.def	wwwdll.def \
	wwwfile.def	wwwftp.def	wwwgophe.def \
	wwwguess.def	wwwhtml.def	wwwhttp.def \
	wwwmime.def	wwwnews.def	wwwrules.def \
	wwwtelnt.def	wwwutils.def	wwwwais.def

EXTRA_DIST = \
	wwwdll.def \
	$(SRC) \
	$(MAK) \
	$(PL)

.files.def:
	./makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def

wwwlist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ./makedefs.pl @ > wwwlist.txt


