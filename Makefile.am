# Process this file with Automake to create Makefile.in.
# (c) COPYRIGHT MIT 1995.
# Please first read the full copyright statement in the file COPYRIGH.
# @(#) $Id$

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
	WELCOME.html \
	README.html \
	INSTALL.html \
	COPYRIGHT.html \
	PATCHES.html \
	CERN.html

# We need to install the config.h header file as well
install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(pkgincludedir)
	$(INSTALL_DATA) $(top_builddir)/$(CONFIG_HEADER) $(pkgincludedir)/$(CONFIG_HEADER);

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign

# These are the sub directories
SUBDIRS = config Library PICS-client LineMode Robot ComLine Icons

# Extra distribution files in zip format and in compress
dist-hook:
	chmod -R a+r $(distdir)
	zip -rq $(distdir).zip $(distdir)
	$(TAR) chof $(distdir).tar $(distdir)
	compress -f $(distdir).tar

# Tag before making distribution.  Also, don't make a distribution if
# checks fail.
CVSSTUFF = config Library PICS-client LineMode Robot ComLine \
Makefile.am acconfig.h acinclude.m4 aclocal.m4 wwwconf.h.in configure.in

# If not indicated otherwise then use this to make the CVS diff with
PREVIOUS_VERSION = @PREVIOUS_VERSION@

cvs-tag:
	cvs -q tag -F `echo "Release-$(VERSION)" | sed 's/\./-/g'` $(CVSSTUFF)

cvs-export-member:
	for subdir in $(SUBDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  rm -rf $$subdir/member;	\
	  cvs export -d $$subdir/member \
		-r `echo "Release-$(VERSION)" | sed 's/\./-/g'` WWW/$$subdir; \
	done && test -z "$$fail"

cvs-export-pub:
	for subdir in $(SUBDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  rm -rf $$subdir/pub;	\
	  cvs export -d $$subdir/pub \
		-r `echo "Release-$(VERSION)" | sed 's/\./-/g'` WWW/$$subdir; \
	done && test -z "$$fail"

cvs-diff:
	thisver=`echo "Release-$(VERSION)" | sed 's/\./-/g'`; \
	prevver=`echo "Release-$(PREVIOUS_VERSION)" | sed 's/\./-/g'`; \
	cvs -f diff -w -c -r $$prevver -r $$thisver $(CVSSTUFF) \
	    > $(PACKAGE)-$(PREVIOUS_VERSION)-$(VERSION).diff

# Generate the include files (normally done automaticly)
inc:
	for subdir in $(SUBDIRS); do		\
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  (cd $$subdir/src && $(MAKE) $$target)	\
	   || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

#
#	Hypertext supplied in text format
#
$(top_srcdir)/WELCOME : $(top_srcdir)/WELCOME.html
	www -p -na $(top_srcdir)/WELCOME.html \
		-o $(top_srcdir)/WELCOME

$(top_srcdir)/README : $(top_srcdir)/README.html
	www -p -na $(top_srcdir)/README.html \
		-o $(top_srcdir)/README

$(top_srcdir)/INSTALL : $(top_srcdir)/INSTALL.html
	www -p -na $(top_srcdir)/INSTALL.html \
		-o $(top_srcdir)/INSTALL

$(top_srcdir)/TODO : $(top_srcdir)/Library/User/ToDo.html
	www -p -na $(top_srcdir)/Library/User/ToDo.html \
		-o $(top_srcdir)/TODO

$(top_srcdir)/NEWS : $(top_srcdir)/Library/User/ReleaseNotes.html
	www -p -na $(top_srcdir)/Library/User/ReleaseNotes.html \
		-o $(top_srcdir)/NEWS

$(top_srcdir)/COPYRIGH : $(top_srcdir)/COPYRIGHT.html
	www -p -na $(top_srcdir)/COPYRIGHT.html \
		-o $(top_srcdir)/COPYRIGH

$(top_srcdir)/PATCHES : $(top_srcdir)/PATCHES.html
	www -p -na $(top_srcdir)/PATCHES.html \
		-o $(top_srcdir)/PATCHES

$(top_srcdir)/CERN : $(top_srcdir)/CERN.html
	www -p -na $(top_srcdir)/CERN.html \
		-o $(top_srcdir)/CERN
