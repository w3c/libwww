#	Makefile for Libwww Examples
#
#	(c) COPYRIGHT MIT 1995.
#	Please first read the full copyright statement in the file COPYRIGH.

#  If this env var is set to something else Some makes will use that instead
SHELL = /bin/sh

include ../../All/$(WWW_MACH)/Makefile.include

#	Directories
CMN = $(WWW)/Library/Implementation
LIB = $(WWW)/Library/$(WWW_MACH)
LIBWWW = $(LIB)/libwww.a

CFLAGS2 = $(CFLAGS) -I$(CMN)
LFLAGS2 = $(LFLAGS) -L$(LIB) -lwww $(SYSLIBS)

#	MODULES
EXP1 = libapp_1
EXP2 = libapp_2
EXP3 = libapp_3
EXP4 = libapp_4

all : $(EXP1) $(EXP2) $(EXP3) $(EXP4)

$(LIBWWW) :
	@echo You must get and make the WWWLibrary product first!
	(cd $(LIB) && make)

$(EXP1) : libapp_1.o $(LIBWWW)
	$(CC) -o $(EXP1) libapp_1.o $(LFLAGS2)

$(EXP2) : libapp_2.o $(LIBWWW)
	$(CC) -o $(EXP2) libapp_2.o $(LFLAGS2)

$(EXP3) : libapp_3.o $(LIBWWW)
	$(CC) -o $(EXP3) libapp_3.o $(LFLAGS2)

$(EXP4) : libapp_4.o $(LIBWWW)
	$(CC) -o $(EXP4) libapp_4.o $(LFLAGS2)

libapp_1.o: libapp_1.c
	$(CC) -c -o $@ $(CFLAGS2) libapp_1.c

libapp_2.o: libapp_2.c
	$(CC) -c -o $@ $(CFLAGS2) libapp_2.c

libapp_3.o: libapp_3.c
	$(CC) -c -o $@ $(CFLAGS2) libapp_3.c

libapp_4.o: libapp_4.c
	$(CC) -c -o $@ $(CFLAGS2) libapp_4.c

clean :
	rm *.o $(EXP1) $(EXP2) $(EXP3) $(EXP4)

