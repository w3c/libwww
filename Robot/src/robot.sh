#!/bin/sh
if [ $# -lt 3 ]
then
	echo "A simple example of how the libwww robot can be used"
	echo "For a full description, see"
	echo
	echo "        http://www.w3.org/Robot/User/CommandLine.html"
	echo
	echo "Usage:	$0 RootURI ImageRootURI LogPrefix [ flags ]"
	echo
        echo "where"
	echo "        RootURI       is the URI prefix for links, for example http://www.w3.org/Robot/"
	echo "        ImageRootURI  is the URI prefix for inlined images, for example http://www.w3.org"
	echo "        LogPrefix     is the prefix for log files, for example robot"
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

${ROBOT} ${FLAGS} -q -ss -n -depth 99 \
-exclude "/ArchiveBrowser/|/History/|/member/|/team/" \
-check "\.gz$|\.tar$|\.tgz$|\.Z$|\.zip$|\.ZIP$|\.exe$|\.EXE$|\.ps$|\.doc$|\.pdf$|\.xplot$|\.java$|\.c$|\.h$|\.txt$|\.ppt$|\.gif$|\.GIF$|\.tiff$|\.png$|\.PNG$|\.jpeg$|\.jpg$|\.JPE$" \
-prefix ${ROOT} \
-img -imgprefix ${IMGROOT} \
-l ${LOG}-log-clf.txt \
-alt ${LOG}-log-alt.txt \
-hit ${LOG}-log-hit.txt \
-rellog ${LOG}-log-link-relations.txt -relation stylesheet \
-lm ${LOG}-log-lastmodified.txt \
-title ${LOG}-log-title.txt \
-referer ${LOG}-log-referer.txt \
-negotiated ${LOG}-log-negotiated.txt \
-404 ${LOG}-log-notfound.txt \
-reject ${LOG}-log-reject.txt \
-format ${LOG}-log-format.txt \
-charset ${LOG}-log-charset.txt \
${ROOT} 
