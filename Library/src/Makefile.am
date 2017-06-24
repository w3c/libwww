## Process this file with Automake to create Makefile.in.

AM_LDFLAGS = -version-info "1:0:1"

lib_LTLIBRARIES = \
	libwwwutils.la \
	libwwwcore.la \
	libwwwtrans.la \
	libwwwstream.la \
	libwwwcache.la \
	libwwwdir.la \
	libwwwfile.la \
	libwwwftp.la \
	libwwwgopher.la \
	libwwwmime.la \
	libwwwhttp.la \
	libwwwnews.la \
	libwwwtelnet.la \
	libwwwhtml.la \
	libwwwapp.la \
	libwwwinit.la \
	libwwwmux.la \
        @WWWDAV@ \
	@WWWXML@ \
	@WWWZIP@ \
	@WWWWAIS@ \
	@WWWSQL@

EXTRA_LTLIBRARIES = \
	libwwwxml.la \
	libwwwzip.la \
	libwwwwais.la \
        libwwwdav.la \
	libwwwsql.la

SUBDIRS = SSL windows vms

#	.h files are distributed but originally are made from the
#	self-documenting hypertext files.
SUFFIXES = .h .html .txt
.html.h:
    ifdef WWW
	$(WWW) -w90 -na -p -to text/x-c $(srcdir)/$(*F).html -o $(srcdir)/$(*F).h
    else
	@echo "**************************************************"
	@echo "Generating the .h files using the "
	@echo "winConfigure.pl script - it may generate a few warnings."
	@echo "**************************************************"
	perl $(top_srcdir)/config/winConfigure.pl $(srcdir)
    endif

icondir = @icondir@
AM_CPPFLAGS := @CPPFLAGS@ -DW3C_ICONS=\"$(icondir)\"

inc: $(BUILT_SOURCES)
	@echo Generated built sources

libwwwutils_la_SOURCES = \
	WWWUtil.h \
	HTArray.h \
	HTArray.c \
	HTAssoc.h \
	HTAssoc.c \
	HTAtom.h \
	HTAtom.c \
	HTBTree.h \
	HTBTree.c \
	HTChunk.h \
	HTChunk.c \
	HTHash.h \
	HTHash.c \
	HTList.h \
	HTList.c \
	HTMemory.h \
	HTMemory.c \
	HTString.h \
	HTString.c \
	HTTrace.c \
	HTUtils.h \
	HTUU.h \
	HTUU.c

libwwwcore_la_SOURCES = \
	WWWCore.h \
	HTAlert.h \
	HTAlert.c \
	HTAncMan.h \
	HTAnchor.c \
	HTAnchor.h \
	HTChannl.h \
	HTChannl.c \
	HTDNS.h \
	HTDNS.c \
	HTError.h \
	HTError.c \
	HTEscape.h \
	HTEscape.c \
	HTEvent.h \
	HTEvent.c \
	HTFormat.c \
	HTFormat.h \
	HTHost.h \
	HTHstMan.h \
	HTHost.c \
	HTInet.h \
	HTInet.c \
	HTLib.h \
	HTLib.c \
	HTLink.h \
	HTLink.c \
	HTMemLog.h \
	HTMemLog.c \
	HTMethod.h \
	HTMethod.c \
	HTNet.h \
	HTNetMan.h \
	HTNet.c \
	HTNoFree.h \
	HTNoFree.c \
	HTParse.h \
	HTParse.c \
	HTProt.h \
	HTProt.c \
	HTReq.h \
	HTReqMan.h \
	HTReqMan.c \
	HTResponse.h \
	HTResMan.h \
	HTResponse.c \
	HTStream.h \
	HTIOStream.h \
	HTStream.c \
	HTStruct.h \
	HTTCP.h \
	HTTCP.c \
	HTTimer.h \
	HTTimer.c \
	HTTrans.h \
	HTTrans.c \
	HTUTree.h \
	HTUTree.c \
	HTUser.h \
	HTUser.c \
	HTWWWStr.h \
	HTWWWStr.c

libwwwtrans_la_SOURCES = \
	WWWTrans.h \
	HTANSI.h \
	HTANSI.c \
	HTBufWrt.h \
	HTBufWrt.c \
	HTLocal.h \
	HTLocal.c \
	HTReader.h \
	HTReader.c \
	HTSocket.h \
	HTSocket.c \
	HTWriter.h \
	HTWriter.c

libwwwstream_la_SOURCES = \
	WWWStream.h \
	HTConLen.h \
	HTConLen.c \
	HTEPtoCl.h \
	HTEPtoCl.c \
	HTFSave.h \
	HTFSave.c \
	HTFWrite.h \
	HTFWrite.c \
	HTGuess.h \
	HTGuess.c \
	HTMerge.h \
	HTMerge.c \
	HTNetTxt.h \
	HTNetTxt.c \
	HTSChunk.h \
	HTSChunk.c \
	HTTee.h \
	HTTee.c \
	HTXParse.h \
	HTXParse.c

libwwwcache_la_SOURCES = \
	WWWCache.h \
	HTCache.h \
	HTCache.c

libwwwfile_la_SOURCES = \
	WWWFile.h \
	HTBInit.h \
	HTBInit.c \
	HTBind.h \
	HTBind.c \
	HTFile.h \
	HTFile.c \
	HTMulti.h \
	HTMulti.c

libwwwftp_la_SOURCES = \
	WWWFTP.h \
	HTFTP.h \
	HTFTP.c \
	HTFTPDir.h \
	HTFTPDir.c

libwwwgopher_la_SOURCES = \
	WWWGophe.h \
	HTGopher.h \
	HTGopher.c

libwwwmime_la_SOURCES = \
	WWWMIME.h \
	HTBound.h \
	HTBound.c \
	HTHeader.h \
	HTHeader.c \
	HTMIME.h \
	HTMIME.c \
	HTMIMPrs.h \
	HTMIMPrs.c \
	HTMIMERq.h \
	HTMIMERq.c \
	HTMIMImp.h \
	HTMIMImp.c \
	HTMulpar.h

libwwwhttp_la_SOURCES = \
	WWWHTTP.h \
	HTTPUtil.h \
	HTAABrow.h \
	HTAABrow.c \
	HTAAUtil.h \
	HTAAUtil.c \
	HTCookie.h \
	HTCookie.c \
	HTDigest.h \
	HTDigest.c \
	HTTChunk.h \
	HTTChunk.c \
	HTTP.h \
	HTTP.c \
	HTTPGen.h \
	HTTPGen.c \
	HTTPReq.h \
	HTTPReq.c \
	HTTPRes.h \
	HTTPRes.c \
	HTTPServ.h \
	HTTPServ.c \
	HTPEP.h \
	HTPEP.c

libwwwhttp_la_DEPENDENCIES = \
	../../modules/md5/libmd5.la

libwwwhttp_la_CPPFLAGS = \
	-I/usr/local/include \
	-I${top_srcdir}/modules/md5

libwwwnews_la_SOURCES = \
	WWWNews.h \
	HTNDir.h \
	HTNDir.c \
	HTNews.h \
	HTNews.c \
	HTNewsLs.h \
	HTNewsLs.c \
	HTNewsRq.h \
	HTNewsRq.c

libwwwtelnet_la_SOURCES = \
	WWWTelnt.h \
	HTTelnet.h \
	HTTelnet.c

libwwwwais_la_SOURCES = \
	WWWWAIS.h \
	HTWAIS.h \
	HTWAIS.c \
	HTWSRC.h \
	HTWSRC.c

libwwwwais_la_LDFLAGS = -rpath $(libdir)

libwwwhtml_la_SOURCES = \
	WWWHTML.h \
	HTMLPDTD.h \
	HTMLPDTD.c \
	SGML.h \
	SGML.c \
	HTMLGen.h \
	HTMLGen.c \
	HTTeXGen.h \
	HTTeXGen.c \
	HTPlain.h \
	HTPlain.c \
	HTML.h \
	HTML.c \
	HText.h \
	HTextImp.h \
	HText.c \
	HTHInit.h \
	HTHInit.c \
	HTStyle.h \
	HTStyle.c

libwwwdir_la_SOURCES = \
	WWWDir.h \
	HTIcons.h \
	HTIcons.c \
	HTDescpt.h \
	HTDescpt.c \
	HTDir.h \
	HTDir.c

libwwwapp_la_SOURCES = \
	WWWApp.h \
	HTAccess.h \
	HTAccess.c \
	HTDialog.h \
	HTDialog.c \
	HTEvtLst.h \
	HTEvtLst.c \
	HTFilter.h \
	HTFilter.c \
	HTHist.h \
	HTHist.c \
	HTInit.h \
	HTInit.c \
	HTHome.h \
	HTHome.c \
	HTLog.h \
	HTLog.c \
	HTProxy.h \
	HTProxy.c \
	HTRules.h \
	HTRules.c

libwwwinit_la_SOURCES = \
	WWWInit.h \
	HTProfil.h \
	HTProfil.c

libwwwmux_la_SOURCES = \
	WWWMux.h \
	HTDemux.h \
	HTDemux.c \
	HTMuxCh.h \
	HTMuxCh.c \
	HTMuxHeader.h \
	HTMuxTx.h \
	HTMuxTx.c

libwwwdav_la_SOURCES = \
        HTDAV.h \
        HTDAV.c

libwwwdav_la_LDFLAGS = -rpath $(libdir)

libwwwxml_la_SOURCES = \
	WWWXML.h \
	HTRDF.h \
	HTRDF.c \
	HTXML.h \
	HTXML.c

libwwwxml_la_DEPENDENCIES =

libwwwxml_la_LDFLAGS = -rpath $(libdir) $(EXPAT_LIBS)

libwwwzip_la_SOURCES = \
	WWWZip.h \
	HTZip.h \
	HTZip.h \
	HTZip.c

libwwwzip_la_LDFLAGS = -rpath $(libdir)

libwwwsql_la_SOURCES = \
	WWWSQL.h \
	HTSQL.h \
	HTSQL.c \
	HTSQLLog.h \
	HTSQLLog.h \
	HTSQLLog.c

libwwwsql_la_LDFLAGS = -rpath $(libdir) $(MYSQL_LIBS)

libwwwsql_la_CPPFLAGS = \
	-I/usr/local/include \
	-I${top_srcdir}/modules/md5 \
	$(MYSQL_CFLAGS)

BUILT_SOURCES = \
	wwwsys.h \
	HTAABrow.h \
	HTAAUtil.h \
	HTANSI.h \
	HTAccess.h \
	HTAlert.h \
	HTAncMan.h \
	HTAnchor.h \
	HTArray.h \
	HTAssoc.h \
	HTAtom.h \
	HTBInit.h \
	HTBTree.h \
	HTBind.h \
	HTBind.h \
	HTBound.h \
	HTBufWrt.h \
	HTCache.h \
	HTChannl.h \
	HTChunk.h \
	HTConLen.h \
	HTCookie.h \
	HTDNS.h \
	HTDemux.h \
	HTDescpt.h \
	HTDialog.h \
	HTDigest.h \
	HTDir.h \
	HTEPtoCl.h \
	HTError.h \
	HTEscape.h \
	HTEvent.h \
	HTEvtLst.h \
	HTFSave.h \
	HTFTP.h \
	HTFTPDir.h \
	HTFWrite.h \
	HTFile.h \
	HTFilter.h \
	HTFormat.h \
	HTGopher.h \
	HTGuess.h \
	HTHInit.h \
	HTHash.h \
	HTHeader.h \
	HTHist.h \
	HTHome.h \
	HTHost.h \
	HTHstMan.h \
	HTIOStream.h \
	HTIcons.h \
	HTInet.h \
	HTInit.h \
	HTLib.h \
	HTLink.h \
	HTList.h \
	HTLocal.h \
	HTLog.h \
	HTMIME.h \
	HTMIMERq.h \
	HTMIMImp.h \
	HTMIMPrs.h \
	HTML.h \
	HTMLGen.h \
	HTMLPDTD.h \
	HTMemLog.h \
	HTMemory.h \
	HTMerge.h \
	HTMethod.h \
	HTMulpar.h \
	HTMulti.h \
	HTMuxCh.h \
	HTMuxHeader.h \
	HTMuxTx.h \
	HTNDir.h \
	HTNet.h \
	HTNetMan.h \
	HTNetTxt.h \
	HTNews.h \
	HTNewsLs.h \
	HTNewsRq.h \
	HTNoFree.h \
	HTPEP.h \
	HTParse.h \
	HTPlain.h \
	HTProfil.h \
	HTProt.h \
	HTProxy.h \
	HTRDF.h \
	HTReader.h \
	HTReq.h \
	HTReqMan.h \
	HTResMan.h \
	HTResponse.h \
	HTRules.h \
	HTSChunk.h \
	HTSQL.h \
	HTSQLLog.h \
	HTSocket.h \
	HTStream.h \
	HTString.h \
	HTStruct.h \
	HTStyle.h \
	HTTCP.h \
	HTTChunk.h \
	HTTP.h \
	HTTPGen.h \
	HTTPReq.h \
	HTTPRes.h \
	HTTPServ.h \
	HTTPUtil.h \
	HTTeXGen.h \
	HTTee.h \
	HTTelnet.h \
	HTTimer.h \
	HTTrans.h \
	HTUTree.h \
	HTUU.h \
	HTUser.h \
	HTUtils.h \
	HTWAIS.h \
	HTWSRC.h \
	HTWWWStr.h \
	HTWriter.h \
	HTXML.h \
	HTXParse.h \
	HTZip.h \
	HText.h \
	HTextImp.h \
        HTDAV.h \
	SGML.h \
	WWWApp.h \
	WWWCache.h \
	WWWCore.h \
	WWWDir.h \
	WWWFTP.h \
	WWWFile.h \
	WWWGophe.h \
	WWWHTML.h \
	WWWHTTP.h \
	WWWInit.h \
	WWWLib.h \
	WWWMIME.h \
	WWWMux.h \
	WWWNews.h \
	WWWSQL.h \
	WWWStream.h \
	WWWTelnt.h \
	WWWTrans.h \
	WWWUtil.h \
	WWWWAIS.h \
	WWWXML.h \
	WWWZip.h

pkginclude_HEADERS = $(BUILT_SOURCES)

DOCS :=	$(wildcard *.html)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(DOCS)
