# Process this file with Automake to create Makefile.in.
# (c) COPYRIGHT MIT 1995.
# Please first read the full copyright statement in the file COPYRIGH.
# @(#) $Id$

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .html

BUILT_SOURCES = \
	COPYRIGH

legal : $(BUILT_SOURCES)
	@echo Generated built sources

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	README.html \
	INSTALL.html \
	COPYRIGHT.html \
	PATCHES.html \
	LICENSE.html \
	config/htmlCase.pl \
	config/splitBigTcpdump.pl \
	config/makeindx.pl \
	config/netmon2tcpdump.pl \
	config/makedefs.pl \
	config/winConfigure.pl \
	config/depcomp \
	config/m4 \
	modules


# We need to install the config.h header file as well. As this file
# is platform specific, we do not install it with the other libwww include files
install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	$(INSTALL_DATA) $(top_builddir)/$(CONFIG_HEADER) $(DESTDIR)$(includedir)/$(CONFIG_HEADER);

# This is our install script provided by Soren Sandmann
bin_SCRIPTS = libwww-config

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I config/m4

# These are the sub directories
SUBDIRS = config modules Library PICS-client LineMode Robot ComLine WinCom Icons

# Extra distribution files in zip format and in compress
dist-hook:
	chmod -R a+r $(distdir)
	zip -rq $(distdir).zip $(distdir)
	$(TAR) chofz $(distdir).tgz $(distdir)
#       gzip -f $(distdir).tar

# Tag before making distribution.  Also, don't make a distribution if
# checks fail.
CVSSTUFF = config modules Library PICS-client LineMode Robot ComLine WinCom Icons \
Makefile.am acconfig.h aclocal.m4 wwwconf.h.in configure.in

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
#$(top_srcdir)/COPYRIGH : $(top_srcdir)/COPYRIGHT.html
#	ifdef WWW
#	  -$(WWW) -p -na $(top_srcdir)/COPYRIGHT.html \
#		  -o $(top_srcdir)/COPYRIGH
#	endif
