#	Makefile for W3C Reference Library DLLs
#	---------------------------------------
#
#	(c) COPYRIGHT MIT 1995.
#	Please first read the full copyright statement in the file COPYRIGH.
#
#______________________________________________________________________
#
#  If this env var is set to something else Some makes will use that instead
SHELL = /bin/sh

MDEF= ./makedefs.pl @

all : def list


def :
	for i in *.files; do $(MDEF) < $$i > `basename $$i files`def; done

list :
	ls -1 ../*.c | sed -e s=\.\./==1 | $(MDEF) > wwwlist.txt 
