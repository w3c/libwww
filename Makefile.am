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
SUBDIRS = Library PICS LineMode Robot ComLine MiniServ Listen Icons

#
#	Hypertext supplied in text format
#
$(top_srcdir)/WELCOME : /afs/w3.org/devel/WWW/WELCOME.html
	www -p -na http://www.w3.org/devel/WWW/WELCOME.html \
		-o $(top_srcdir)/WELCOME

$(top_srcdir)/README : /afs/w3.org/devel/WWW/README.html
	www -p -na http://www.w3.org/devel/WWW/README.html \
		-o $(top_srcdir)/README

$(top_srcdir)/INSTALL : /afs/w3.org/devel/WWW/INSTALL.html
	www -p -na http://www.w3.org/devel/WWW/INSTALL.html \
		-o $(top_srcdir)/INSTALL

$(top_srcdir)/NEWS : /afs/w3.org/devel/WWW/Library/User/ReleaseNotes.html
	www -p -na http://www.w3.org/devel/WWW/Library/User/ReleaseNotes.html \
		-o $(top_srcdir)/NEWS

$(top_srcdir)/COPYRIGH : /afs/w3.org/devel/WWW/COPYRIGHT.html
	www -p -na http://www.w3.org/devel/WWW/COPYRIGHT.html \
		-o $(top_srcdir)/COPYRIGH

$(top_srcdir)/PATCHES : /afs/w3.org/devel/WWW/PATCHES.html
	www -p -na http://www.w3.org/devel/WWW/PATCHES.html \
		-o $(top_srcdir)/PATCHES

$(top_srcdir)/CERN : /afs/w3.org/devel/WWW/CERN.html
	www -p -na http://www.w3.org/devel/WWW/CERN.html \
		-o $(top_srcdir)/CERN
