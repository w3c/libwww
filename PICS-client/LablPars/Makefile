#	Makefile for PICS Lable Parser example
#
#	(c) COPYRIGHT MIT 1996.
#	Please first read the full copyright statement in the file COPYRIGH.
#
# Macros required to be defined for make:
#
# CC		The C compiler
# CFLAGS	Flags for $(CC)
# LFLAGS	Flags for ld
# WWW		Root of WWW source tree
#______________________________________________________________________

WWW = ../..
#	Directories
OBJ = $(WWW)/PICS-client/LablPars
CMN = $(WWW)/Library/src
PICS= $(WWW)/PICS-client/src
CL  = $(WWW)/PICS-client/LablPars
#	Or, if compiling without libwww, use these instead
#OBJ = .
#CMN = ../HTUtils
#PICS= ../src
#CL  = .
#	Configuration
CC  = gcc
# use the config.h from the LablPars directory
#CFG =  -I. -DHAVE_CONFIG_H
# or bypass config.h and define what it would have
# this example works for solaris
CFG = -DSTDC_HEADERS -DHAVE_MEMCPY -DHAVE_STDIO_H

CFLAGS = -I$(CMN) -I$(PICS) $(CFG)
LFLAGS =

#	MODULES
OBJS =	$(OBJ)/CSLabel.o \
	$(OBJ)/CSParse.o \
	$(OBJ)/HTChunk.o \
	$(OBJ)/HTList.o \
	$(OBJ)/HTString.o \
	$(OBJ)/LablPars.o

CFILES =$(PICS)/CSLabel.c \
	$(PICS)/CSParse.c \
	$(CMN)/HTChunk.c \
	$(CMN)/HTList.c \
	$(CMN)/HTString.c \
	$(CL)/LablPars.c

HFILES=	$(PICS)/CSLL.h \
	$(PICS)/CSLLSt.h \
	$(PICS)/CSLUtils.h \
	$(PICS)/CSParse.h \
	$(CMN)/HTChunk.h \
	$(CMN)/HTList.h \
	$(CMN)/HTMemory.h \
	$(CMN)/HTString.h \
	$(CMN)/HTUtils.h

LablPars : $(OBJS)
	$(CC) -o LablPars $(OBJS) $(LFLAGS)

$(OBJ)/CSLabel.o : $(PICS)/CSLabel.c $(CMN)/HTString.h \
	$(PICS)/CSParse.h $(PICS)/CSLUtils.h $(PICS)/CSLL.h $(PICS)/CSLLSt.h
	$(CC) -c -o $@ $(CFLAGS) $(PICS)/CSLabel.c

$(OBJ)/CSParse.o : $(PICS)/CSParse.c $(CMN)/HTChunk.h \
	$(CMN)/HTString.h $(PICS)/CSLUtils.h $(PICS)/CSParse.h
	$(CC) -c -o $@ $(CFLAGS) $(PICS)/CSParse.c

$(OBJ)/HTChunk.o : $(CMN)/HTChunk.c $(CMN)/HTUtils.h $(CMN)/HTChunk.h
	$(CC) -c -o $@ $(CFLAGS) $(CMN)/HTChunk.c

$(OBJ)/HTList.o : $(CMN)/HTList.c $(CMN)/HTUtils.h $(CMN)/HTList.h
	$(CC) -c -o $@ $(CFLAGS) $(CMN)/HTList.c

$(OBJ)/HTString.o : $(CMN)/HTString.c $(CMN)/HTUtils.h $(CMN)/HTString.h
	$(CC) -c -o $@ $(CFLAGS) $(CMN)/HTString.c

$(OBJ)/LablPars.o : $(CL)/LablPars.c \
	$(PICS)/CSLUtils.h $(PICS)/CSLL.h $(PICS)/CSParse.h
	$(CC) -c -o $@ $(CFLAGS) $(CL)/LablPars.c


