## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def .txt

DEF := $(wildcard *.def)
MAK := $(wildcard *.mak)
PL  := $(wildcard *.pl)

DIST_OTHER = \
	$(DEF) \
	$(MAK) \
	$(PL)

def:
	for i in *.files; do ./makedefs.pl @ < $$i > `basename $$i files`def; done

list:
	ls -1 ../*.c | sed -e s=\.\./==1 | ./makedefs.pl @ > wwwlist.txt 

.files.def:
	./makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def

wwwlist.txt:
	ls -1 ../*.c | sed -e s=\.\./==1 | ./makedefs.pl @ > wwwlist.txt

windows: def list

