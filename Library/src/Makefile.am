## Process this file with Automake to create Makefile.in.

lib_LIBRARIES = www

SUBDIRS = windows vms

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html .txt
.html.h:
	www -w90 -na -p -to text/x-c $(srcdir)/$(*F).html > $(srcdir)/$(*F).h

inc: $(BUILT_SOURCES)
	@echo Generated built sources

AUTOMAKE_OPTIONS = foreign

www_SOURCES = \
	HTParse.c	HTAccess.c 	HTProxy.c \
	HTMethod.c 	HTProt.c 	HTCache.c \
	HTReqMan.c 	HTDNS.c 	HTArray.c \
        HTFile.c 	HTBTree.c 	HTMulti.c \
	HTFTP.c 	HTTCP.c     	SGML.c \
	HTML.c		HTMLPDTD.c 	HTDir.c \
	HTFTPDir.c	HTEPtoCl.c	HTStyle.c \
	HTPlain.c	HTWriter.c  	HTFWrite.c \
        HTMLGen.c	HTTee.c 	HTXParse.c \
	HTChunk.c 	HTAtom.c   	HTAnchor.c \
	HTList.c  	HTString.c 	HTAlert.c \
	HTFormat.c 	HTHist.c 	HTMemory.c \
	HTInit.c 	HTBInit.c  	HTRules.c \
	HTMIME.c 	HTMIMERq.c 	HTBound.c \
	HTConLen.c 	HTHeader.c	HTBind.c \
	HTGopher.c 	HTTelnet.c	HTNetTxt.c \
	HTNews.c  	HTWSRC.c 	HTSocket.c \
	HTAAUtil.c 	HTAABrow.c 	HTAssoc.c \
	HTUU.c		HTTeXGen.c	HTEscape.c \
	HTDescpt.c 	HTGuess.c 	HTWWWStr.c \
	HTIcons.c 	HTError.c 	HTLog.c \
	HTHome.c 	HTDialog.c	HTNDir.c \
	HTTP.c		HTTPServ.c	HTNewsRq.c \
	HTTPGen.c 	HTTPReq.c 	HTTPRes.c \
	HTNet.c 	HTEvntrg.c	HTNewsLs.c \
	HTUser.c	HTMIMPrs.c	HTEvent.c \
	HTMux.c		HTStream.c	HTTrans.c \
	HTChannl.c	HTInet.c	HTHost.c \
	HTLocal.c	HTReader.c	HTANSI.c \
	HTBufWrt.c	HTTChunk.c	HTHInit.c \
	HTMIMImp.c	HTSChunk.c	HTLib.c \
	HTLink.c 	HTFilter.c	HTUTree.c \
	HTPEP.c		HTProfil.c	HTMerge.c \
	HTResponse.c

www_LIBADD = @HTWAIS@

EXTRA_www_SOURCES = \
	HTWAIS.c

#
#	Kludge for finding icons
#
icondir = $(datadir)/@icondir@
INCLUDES = -DW3C_ICONS=\"$(icondir)\"

BUILT_SOURCES = sysdep.h \
	WWWUtil.h 	WWWCore.h 	WWWLib.h \
	WWWDir.h 	WWWFTP.h 	WWWFile.h \
	WWWStream.h 	WWWHTML.h 	WWWHTTP.h \
	WWWNews.h 	WWWTrans.h 	WWWTelnt.h \
	WWWCache.h 	WWWApp.h 	WWWInit.h \
	WWWGophe.h	WWWMIME.h	WWWWAIS.h \
	HTParse.h 	HTAccess.h  	HTProxy.h \
	HTMethod.h 	HTProt.h 	HTCache.h \
	HTReqMan.h 	HTReq.h  	HTDNS.h \
	HTMulti.h 	HTBTree.h 	HTFTP.h \
	SGML.h		HTML.h		HTMLPDTD.h \
	HTNet.h 	HTNetMan.h 	HTFTPDir.h \
	HTPlain.h	HTWriter.h	HTFile.h \
	HTFWrite.h 	HTMLGen.h	HTTCP.h \
	HTEvntrg.h 	HTArray.h  	HTChunk.h \
	HTUser.h	HTMIMPrs.h	HTEvent.h \
	HTStream.h 	HTStruct.h 	HTMulpar.h \
	HTTee.h 	HTXParse.h 	HTEPtoCl.h \
	HTAtom.h 	HTAnchor.h 	HTAncMan.h \
	HTList.h 	HTWWWStr.h 	HTMemory.h \
	HTString.h 	HTAlert.h 	HTRules.h \
	HTFormat.h  	HTStyle.h	HTNewsLs.h \
	HTInit.h 	HTBInit.h	HTBind.h \
	HTMIME.h 	HTMIMERq.h 	HTBound.h \
	HTConLen.h 	HTHeader.h	HTHist.h \
	HTGopher.h 	HTDir.h		HTUtils.h \
 	HTNewsRq.h	HText.h 	HTTelnet.h \
	HTNDir.h  	HTWSRC.h 	HTSocket.h \
	HTAAUtil.h 	HTAABrow.h 	HTAssoc.h \
	HTUU.h		HTTeXGen.h	HTDescpt.h \
	HTGuess.h	HTNetTxt.h 	HTBind.h \
	HTIcons.h 	HTError.h 	HTLog.h \
	HTHome.h 	HTDialog.h 	HTEscape.h \
	HTTP.h		HTTPServ.h	HTNews.h \
	HTTPGen.h 	HTTPReq.h 	HTTPRes.h \
	HTTPUtil.h	HTMux.h		HTTrans.h \
	HTIOStream.h	HTChannl.h	HTInet.h \
	HTHost.h	HTLocal.h	HTReader.h \
	HTANSI.h	HTBufWrt.h	HTTChunk.h \
	HTHInit.h	HTMIMImp.h	HTSChunk.h \
	HTLib.h		HTLink.h	HTFilter.h \
	HTUTree.h	HTPEP.h		HTProfil.h \
	HTMerge.h	HTResponse.h	HTResMan.h

EXTRA_HEADERS = \
	HTWAIS.h

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS)
