## Process this file with Automake to create Makefile.in.
##
## This is not up-to-date with the latest API so we skip it for now
##

# bin_PROGRAMS = w3clib

w3clibLDADD = \
	@LIBTCL@ \
	../../src/libwww.a -lm

w3clibDEPENDENCIES = \
	@LIBTCL@ \
	../../src/libwww.a

#	For easier use - NOT THE DEFAULT!
AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(srcdir)/../../src

w3clibSOURCES = \
	HTAnchor_glue.c 	HTAnchor_glue.h \
	HTReq_glue.c 		HTReq_glue.h \
	HTList_glue.c		HTList_glue.h \
	HTHost_glue.c		HTHost_glue.h \
	HTLog_glue.c 		HTLog_glue.h \
	HTFWriter_glue.c	HTFWriter_glue.h \
	HTAssoc_glue.c		HTAssoc_glue.h \
	HTBind_glue.c		HTBind_glue.h \
	HTProt_glue.c		HTProt_glue.h \
	HTUser_glue.c		HTUser_glue.h \
	HTLink_glue.c		HTLink_glue.h \
	HTAccess_glue.c		HTAccess_glue.h \
	HTParse_glue.c		HTParse_glue.h \
	HTFormat_glue.c		HTFormat_glue.h \
	HTError_glue.c		HTError_glue.h \
	HTAlert_glue.c		HTAlert_glue.h \
	HTDNS_glue.c		HTDNS_glue.h \
	HTUTree_glue.c		HTUTree_glue.h \
	HTTrans_glue.c		HTTrans_glue.h \
	HTNet_glue.c		HTNet_glue.h \
	HTInet_glue.c		HTInet_glue.h \
	Libdata.c		Libdata.h \
	glue.c			glue.h \
	URLgen.c		URLgen.h \
	Tcl_Interface.c		Tcl_Interface.h \
	Enumerations.c		Enumerations.h \
	tclAppInit.c




