## Process this file with Automake to create Makefile.in.

ICON= $(top_srcdir)/Icons
FTP = /afs/w3.org/devel/Dist/$(PROD)

legal : $(LEGAL)

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html .txt

.html.txt:
	www -n -p $< > $@

.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$*.html > $(srcdir)/$*.h

#	Some extra make directives
inc-recursive:
	for subdir in $(SUBDIRS); do		\
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  (cd $$subdir && $(MAKE) $$target)	\
	   || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
inc : inc-recursive
	@echo Include files generated from hypertext.

all-local : inc legal

LEGAL =	$(top_srcdir)/../WELCOME \
	$(top_srcdir)/README \
	$(top_srcdir)/INSTALL \
	$(top_srcdir)/COPYRIGH \
	$(top_srcdir)/PATCHES \
	$(top_srcdir)/CERN

ICONS = $(top_srcdir)/Icons/WWW/Bug48x.gif \
	$(top_srcdir)/Icons/WWW/Lib48x.gif \
	$(top_srcdir)/Icons/WWW/w3c_48x48.gif \
	$(top_srcdir)/Icons/WWW/Platform48x.gif \
	$(top_srcdir)/Icons/WWW/design48x.gif \
	$(top_srcdir)/Icons/WWW/doc48x.gif \
	$(top_srcdir)/Icons/WWW/guide48x.gif \
	$(top_srcdir)/Icons/WWW/internals48x.gif \
	$(top_srcdir)/Icons/WWW/install48x.gif \
	$(top_srcdir)/Icons/WWW/relnotes48x.gif \
	$(top_srcdir)/Icons/WWW/bullet.gif \
	$(top_srcdir)/Icons/32x32/smiley.gif \
	$(top_srcdir)/Icons/32x32/caution.gif

DIST_OTHER = \
	$(LEGAL) \
	$(ICONS)

SUBDIRS = Library PICS LineMode Robot ComLine MiniServ Listen

#
#	Hypertext supplied in text format
#
$(top_srcdir)/../WELCOME : /afs/w3.org/devel/WELCOME.html
	www -p -na http://www.w3.org/devel/WELCOME.html \
		-o $(top_srcdir)/../WELCOME

$(top_srcdir)/README : /afs/w3.org/devel/WWW/README.html
	www -p -na http://www.w3.org/devel/WWW/README.html \
		-o $(top_srcdir)/README

$(top_srcdir)/INSTALL : /afs/w3.org/devel/WWW/INSTALL.html
	www -p -na http://www.w3.org/devel/WWW/INSTALL.html \
		-o $(top_srcdir)/INSTALL

$(top_srcdir)/COPYRIGH : /afs/w3.org/devel/WWW/COPYRIGHT.html
	www -p -na http://www.w3.org/devel/WWW/COPYRIGHT.html \
		-o $(top_srcdir)/COPYRIGH

$(top_srcdir)/PATCHES : /afs/w3.org/devel/WWW/PATCHES.html
	www -p -na http://www.w3.org/devel/WWW/PATCHES.html \
		-o $(top_srcdir)/PATCHES

$(top_srcdir)/CERN : /afs/w3.org/devel/WWW/CERN.html
	www -p -na http://www.w3.org/devel/WWW/CERN.html \
		-o $(top_srcdir)/CERN
