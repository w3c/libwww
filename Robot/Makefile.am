## Process this file with Automake to create Makefile.in.

DOC = User
EXP = Examples

EXAMPLES =

DOCS =	*.html \
	`find $(DOC) -follow \( -name \*.html -o -name \*.gif \) -print` \
	$(DOC)/Patch/*.fix

DIST_OTHER = \
	$(DOCS)

SUBDIRS = src
