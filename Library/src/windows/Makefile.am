## Process this file with Automake to create Makefile.in.

SUFFIXES = .files .def

SRC := $(wildcard *.c)
MAK := $(wildcard *.mak)
# PL  := $(wildcard *.pl) I don't think we need this anymore - EGP

AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
	wwwapp.def	wwwapp.def	wwwcache.def \
	wwwcore.def	wwwdir.def \
	wwwfile.def	wwwftp.def	wwwgophe.def \
	wwwstream.def	wwwhtml.def	wwwhttp.def \
	wwwmime.def	wwwnews.def	wwwrules.def \
	wwwtelnt.def	wwwutils.def	wwwwais.def \
	wwwtrans.def

EXTRA_DIST = \
	wwwdll.def \
	$(SRC) \
	$(MAK)
#	$(PL) - EGP

def:  maintainer-clean-generic $(BUILT_SOURCES)
	
.files.def:
	$(top_srcdir)/config/makedefs.pl @ < $(srcdir)/$(*F).files > $(srcdir)/$(*F).def



