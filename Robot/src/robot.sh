#!/bin/sh
if [ $# -lt 3 ]
then
	echo
	echo "Usage:	$0 RootURI ImageRootURI LogPrefix [ flags ]"
	echo
        echo "where"
	echo "        RootURI       is the URI prefix for links, for example http://www.w3.org"
	echo "        ImageRootURI  is the URI prefix for inlined images, for example http://www.w3.org"
	echo "        LogPrefix     is the prefix for log files, for example w3c"
	echo "        flags         are any additional command line flags, for example -vp"
	echo
	echo "See"
	echo "        http://www.w3.org/Robot/"
	echo
	echo "for more information"
	exit 1
fi

ROOT=$1
IMGROOT=$2
LOG=$3
FLAGS=$4

ROBOT=./webbot

${ROBOT} ${FLAGS} -ss -q -n -depth 1 \
-exclude "/ArchiveBrowser/|/History/|/member/|/team/" \
-check "\.gz$|\.Z$|\.zip$|\.ZIP$|\.tar$|\.ps$|\.doc$|\.pdf$|\.xplot$|\.java$|\.c$|\.h$|\.txt$|\.ppt$|\.gif$|\.GIF$|\.tiff$|\.png$|\.PNG$|\.jpeg$|\.jpg$|\.JPE$" \
-prefix ${ROOT} \
-img -imgprefix ${IMGROOT} \
-l ${LOG}-log-clf.txt \
-alt ${LOG}-log-alt.txt \
-hit ${LOG}-log-hit.txt \
-lm ${LOG}-log-lastmodified.txt \
-title ${LOG}-log-title.txt \
-referer ${LOG}-log-referer.txt \
-negotiated ${LOG}-log-negotiated.txt \
-404 ${LOG}-log-notfound.txt \
-reject ${LOG}-log-reject.txt \
-format ${LOG}-log-format.txt \
-charset ${LOG}-log-charset.txt \
${ROOT} 
