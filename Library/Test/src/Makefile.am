## Process this file with Automake to create Makefile.in.

bin_PROGRAMS = w3clib

w3clib_LDADD = \
	@LIBTCL@ \
	../../src/libwww.a ../../../PICS-client/src/libpics.a -lm

w3clib_DEPENDENCIES = \
	@LIBTCL@ \
	../../src/libwww.a ../../../PICS-client/src/libpics.a

#	For easier use - NOT THE DEFAULT!
AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../../../PICS-client/src

w3clib_SOURCES = \
	HTAnchor_glue.c HTReq_glue.c HTList_glue.c HTHost_glue.c HTLog_glue.c\
	HTFWriter_glue.c HTAssoc_glue.c HTBind_glue.c HTProt_glue.c\
	HTUser_glue.c HTLink_glue.c HTAccess_glue.c HTParse_glue.c \
	glue.c Libdata.c Enumerations.c tclAppInit.c URLgen.c HTFormat_glue.c\
	HTError_glue.c HTAlert_glue.c






