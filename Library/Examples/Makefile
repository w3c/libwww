#	Makefile for Libwww Examples
#
#	(c) COPYRIGHT MIT 1995.
#	Please first read the full copyright statement in the file COPYRIGH.

#  If this env var is set to something else Some makes will use that instead
SHELL = /bin/sh

CC = gcc

#	Directories
LIB = ../$(WWW_MACH)
CMN = ../Implementation

CFLAGS = -I$(CMN)
LFLAGS = -L$(LIB) -lwww

#	MODULES
EXP1 = libapp_1
EXP2 = libapp_2
EXP3 = libapp_3

all : $(EXP1) $(EXP2) $(EXP3)

$(EXP1) : libapp_1.o $(LIBWWW)
	$(CC) -o $(EXP1) libapp_1.o $(LFLAGS)

$(EXP2) : libapp_2.o $(LIBWWW)
	$(CC) -o $(EXP1) libapp_2.o $(LFLAGS)

$(EXP1) : libapp_3.o $(LIBWWW)
	$(CC) -o $(EXP1) libapp_3.o $(LFLAGS)

libapp_1.o: libapp_1.c
	$(CC) -c -o $@ $(CFLAGS) libapp_1.c

libapp_2.o: libapp_2.c
	$(CC) -c -o $@ $(CFLAGS) libapp_2.c

libapp_3.o: libapp_3.c
	$(CC) -c -o $@ $(CFLAGS) libapp_3.c
