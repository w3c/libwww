## Process this file with Automake to create Makefile.in.

FTP = /afs/w3.org/devel/Dist/$(PROD)

AUTOMAKE_OPTIONS = foreign

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .html

BUILT_SOURCES = \
	WELCOME \
	README \
	INSTALL \
	NEWS \
	TODO \
	COPYRIGH \
	PATCHES \
	CERN

legal : $(BUILT_SOURCES)
	@echo Generated built sources

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	INSTALL.html \
	README.html

#
#	We do not have _all_ GNU files so add this to avoid warnings
#
AUTOMAKE_OPTIONS = foreign

#
#	These are the sub directories
#
SUBDIRS = config Library PICS-client LineMode Robot ComLine MiniServ Listen Icons

#	Extra distribution files in zip format and in compress
dist-hook:
	chmod -R a+r $(distdir)
	zip -rq $(distdir).zip $(distdir)
	$(TAR) chof $(distdir).tar $(distdir)
	compress -f $(distdir).tar

# Tag before making distribution.  Also, don't make a distribution if
# checks fail.  Also, make sure the NEWS file is up-to-date.
CVSDIRS = Library LineMode Robot ComLine MiniServ Listen
cvs-tag:
	cvs -q tag -F `echo "Release-$(VERSION)" | sed 's/\./-/g'` $(CVSDIRS)

cvs-export-member:
	for subdir in $(CVSDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  rm -rf $$subdir/member;	\
	  cvs export -d $$subdir/member \
		-r `echo "Release-$(VERSION)" | sed 's/\./-/g'` $$subdir; \
	done && test -z "$$fail"

cvs-export-pub:
	for subdir in $(CVSDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  rm -rf $$subdir/pub;	\
	  cvs export -d $$subdir/pub \
		-r `echo "Release-$(VERSION)" | sed 's/\./-/g'` $$subdir; \
	done && test -z "$$fail"

cvs-diff:
	thisver=`echo "Release-$(VERSION)" | sed 's/\./-/g'`; \
	prevno=`echo "$(VERSION)" - 0.01 | bc | sed 's/^\./0./'`; \
	prevver=Release-`echo $$prevno | sed 's/\./-/g'`; \
	cvs -f rdiff -c -r $$prevver -r $$thisver $(PACKAGE) \
	    > $(PACKAGE)-$$prevno-$(VERSION).diff

FULLDIRS = $(CVSDIRS) PICS-client
inc:
	for subdir in $(FULLDIRS); do		\
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  (cd $$subdir/src && $(MAKE) $$target)	\
	   || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

#
#	Hypertext supplied in text format
#
$(top_srcdir)/WELCOME : /afs/w3.org/pub/WWW/WELCOME.html
	www -p -na /afs/w3.org/pub/WWW/WELCOME.html \
		-o $(top_srcdir)/WELCOME

$(top_srcdir)/README : /afs/w3.org/pub/WWW/README.html
	www -p -na /afs/w3.org/pub/WWW/README.html \
		-o $(top_srcdir)/README

$(top_srcdir)/INSTALL : /afs/w3.org/pub/WWW/INSTALL.html
	www -p -na /afs/w3.org/pub/WWW/INSTALL.html \
		-o $(top_srcdir)/INSTALL

$(top_srcdir)/TODO : /afs/w3.org/pub/WWW/Library/User/ToDo.html
	www -p -na /afs/w3.org/pub/WWW/Library/User/ToDo.html \
		-o $(top_srcdir)/TODO

$(top_srcdir)/NEWS : /afs/w3.org/pub/WWW/Library/User/ReleaseNotes.html
	www -p -na /afs/w3.org/pub/WWW/Library/User/ReleaseNotes.html \
		-o $(top_srcdir)/NEWS

$(top_srcdir)/COPYRIGH : /afs/w3.org/pub/WWW/COPYRIGHT.html
	www -p -na /afs/w3.org/pub/WWW/COPYRIGHT.html \
		-o $(top_srcdir)/COPYRIGH

$(top_srcdir)/PATCHES : /afs/w3.org/pub/WWW/PATCHES.html
	www -p -na /afs/w3.org/pub/WWW/PATCHES.html \
		-o $(top_srcdir)/PATCHES

$(top_srcdir)/CERN : /afs/w3.org/pub/WWW/CERN.html
	www -p -na /afs/w3.org/pub/WWW/CERN.html \
		-o $(top_srcdir)/CERN
