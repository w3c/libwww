## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
PL  := $(wildcard *.pl)

AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
	wwwapp.def	wwwapp.def	wwwcache.def \
	wwwcore.def	wwwdir.def	wwwdll.def \
	wwwfile.def	wwwftp.def	wwwgophe.def \
	wwwstream.def	wwwhtml.def	wwwhttp.def \
	wwwmime.def	wwwnews.def	wwwrules.def \
	wwwtelnt.def	wwwutils.def	wwwwais.def \
	wwwtrans.def	wwwlist.txt

EXTRA_DIST = \
	wwwdll.def \
	$(SRC) \
	$(MAK) \
	$(PL)

.files.def:
	./makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def

wwwlist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ./makedefs.pl @ > wwwlist.txt


