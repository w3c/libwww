## Process this file with Automake to create Makefile.in.

FTP = /afs/w3.org/devel/Dist/$(PROD)

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html

AUTOMAKE_OPTIONS = foreign

.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

LEGAL =	WELCOME \
	README \
	INSTALL \
	NEWS \
	COPYRIGH \
	PATCHES \
	CERN

legal : $(LEGAL)

EXTRA_DIST = \
	INSTALL.html \
	README.html \
	$(LEGAL)

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
CVSDIRS = Library PICS-client LineMode Robot ComLine MiniServ Listen
cvs-branch:
	@if sed 1q $(srcdir)/NEWS | egrep -e "$(VERSION)" > /dev/null; then :; else \
	  echo "NEWS not updated; not releasing" 1>&2; \
	  exit 1;				\
	fi
	cvs -q tag -b `echo "Release-$(VERSION)" | sed 's/\./-/g'` $(CVSDIRS)

cvs-diff:
	thisver=`echo "Release-$(VERSION)" | sed 's/\./-/g'`; \
	prevno=`echo "$(VERSION)" - 0.01 | bc | sed 's/^\./0./'`; \
	prevver=Release-`echo $$prevno | sed 's/\./-/g'`; \
	cvs -f rdiff -c -r $$prevver -r $$thisver $(PACKAGE) \
	    > $(PACKAGE)-$$prevno-$(VERSION).diff

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
