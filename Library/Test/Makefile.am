## Process this file with Automake to create Makefile.in.

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src User

# target name
PROG= ./dejagnu/expect/expect
WWWTEST = wwwtest
SRCDIR = .

# testsuite specifics
RUNTEST= ./dejagnu/dejagnu/runtest
RUNTESTFLAGS= -v -v
#-all

TEST_FILES=
#HTAnchor.exp

check:	site.exp
	rootme=`pwd`; export rootme; \
	EXPECT=${PROG} ${RUNTEST} ${RUNTESTFLAGS} --tool ${WWWTEST} ${TEST_FILES} --srcdir ${SRCDIR}/testsuite WWWTEST=${WWWTEST}
