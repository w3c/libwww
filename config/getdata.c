/*
**  This is a small program to gather statistical data from tcpdump
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define ROBOT_PATH	"/home2/frystyk"
#define ROBOT_HTTP10	"w3c-libwww-4.0D"
#define ROBOT_HTTP11	"w3c-libwww"
#define ROBOT_HTTP11PL	"w3c-libwww"
#define ROBOT		"Robot/src/webbot"

#define DEFAULT_HOST	"pfunk.w3.org"
#define DEFAULT_PORT	"8080"
#define DEFAULT_HOME	"/microscape/microscape.html"
#define ZIP_HOME	"/microscape/m.d.html"
#define DEFAULT_SLEEP	5

#define DEFAULT_LOG	"tcpdump"
#define TCPDUMP		"/usr/local/sbin/tcpdump"

#define TEXT_SUFFIX	"txt"
#define BIN_SUFFIX	"bin"

#define	YES	1
#define NO	0

typedef enum _ProgMask {
    HTTP10	= 0x1,
    HTTP11	= 0x2,
    HTTP11PL	= 0x4,
    HTTP11PLZIP	= 0x8,
    HTTPALL	= 0xFF
} ProgMask;

extern char ** environ;

static char * tcpdump = TCPDUMP;
static char * robotpath = ROBOT_PATH;
static int    zzzz = DEFAULT_SLEEP;

void HelpScreen (const char *progname)
{
    fprintf(stderr, "\nA small program to gather statistical data from tcpdump\n");
    fprintf(stderr, "Usage: %s\n", progname ? progname : "getdata");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-d\tDelay between each dump in seconds (default 5)\n");
    fprintf(stderr, "\t-l\tLogfile prefix, for example 'J-HH'\n");
    fprintf(stderr, "\t-o\tOther robot 5.1 command line options, for example '-delay 20'\n");
    fprintf(stderr, "\t-p\tPort on host, for example '8080'\n");
    fprintf(stderr, "\t-r\tPath for where to find the Robot executables, for example '/u1/frystyk'\n");
    fprintf(stderr, "\t-s\tHost we are connection to, for example 'pfunk.w3.org'\n");
    fprintf(stderr, "\t-t\tFull path for tcpdump program, for example '/usr/sbin/tcpdump'\n");
    fprintf(stderr, "\t-b\tGenerate binary tcpdump outputs instead of ascii\n");
    fprintf(stderr, "\t-w\tWhat tests to run (1=HTTP/1.0, 2=HTTP/1.1, 4=HTTP/1.1pl). Default is all\n\n");
}

static int CreateLogDir (const char * logdir)
{
    if (logdir) {
	struct stat stat_info;
	if (stat(logdir, &stat_info) == -1) {
	    if (mkdir(logdir, 0777) < 0) return -1;
	}
	return 0;
    }
    return -1;
}

static char ** BuildArgv (const char * host, const char * port, char binary)
{
    char ** argv = NULL;
    if ((argv = (char **) calloc(20, sizeof(char *))) == NULL)
	exit (-10);
    *(argv+0) = "tcpdump";
    if (binary) {
	*(argv+1) = "-w";
	*(argv+2) = "-";
    } else {
	*(argv+1) = "-tt";
	*(argv+2) = "-S";
    }
    *(argv+3) = "host";
    *(argv+4) = host;
    *(argv+5) = "and";
    *(argv+6) = "port";
    *(argv+7) = port;
    return argv;
}


static int RunTest (const char * program, const char * url,
		    const char * host, const char * port,
		    const char * param, const char * log,
		    char binary)
{
    int pid;
    if (program && host && port && log) {

	/* Set up tcpdump to run in a child progess */
	if ((pid = vfork()) < 0) {
	    fprintf(stderr, "Can't fork child process\n");
	    exit(-10);
	}
	if (pid == 0) {
	    char **argv = NULL;
	    int fd = -1;

	    /* Build the calling parameters */
	    argv = BuildArgv(host, port, binary);

	    fprintf(stderr, "Child running as pid %d\n", getpid());

	    /* Open log file and redirect stdout to it */

	    if ((fd = open(log, O_WRONLY | O_CREAT | O_TRUNC,
			   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)) < 0) {
		fprintf(stderr, "Can't open file '%s'\n", log);
	        exit(-5);
	    }
	    dup2(fd, STDOUT_FILENO);
	    close(fd);
		    
	    /* Call tcpdump */
	    if (execve(tcpdump, argv, environ)) {
		fprintf(stderr, "Can't run tcpdump program '%s'\n", tcpdump);
		exit (-5);
	    }
	    fprintf(stderr, "Shouldn't have come here\n");
	    exit (5);
			     
	} else {
	    char cmd[512];
	    fprintf(stderr, "Sleeping for %d secs\n", zzzz);
	    sleep(zzzz);	/* Just to be on the safe side */
	    sprintf(cmd, "%s %s %s", program, param ? param : "", url);
	    fprintf(stderr, "Parent running command '%s'\n", cmd);
	    if (system(cmd)) {
		fprintf(stderr, "Couldn't run command '%s'\n", cmd);
		exit(-20);
	    }
	    fprintf(stderr, "Sleeping for %d secs\n", zzzz);
	    sleep(zzzz);	/* Just to be on the safe side */

	    /* Now kill the child */
	    {
		int status_location;
		fprintf(stderr, "Killing child with pid %d\n", pid);
		if (kill(pid, SIGINT))
		    fprintf(stderr, "Can't kill pid %d\n", pid);
		waitpid(pid, &status_location, 0);
	    }
	}
	return 0;
    }
    return -1;
}

int main (int argc, char ** argv)
{
    int arg;
    char program [512];
    char logdir [512];
    char logpath [512];
    char url [512];
    char param[512];
    char binary = NO;
    char * logprefix = DEFAULT_LOG;
    char * host = DEFAULT_HOST;
    char * port = DEFAULT_PORT;
    char * options = NULL;
    ProgMask mask = HTTPALL;
    char * suffix = TEXT_SUFFIX;

    /* Print help screen and exit */
    if (argc < 2) {
	HelpScreen(*argv);
	exit(0);
    }

    /* Scan command line for parameters */
    for (arg=1; arg<argc; arg++) {
	
	if (*argv[arg] == '-') {
    
	    /* What server are we talking to? */
	    if (!strcmp(argv[arg], "-s")) {
		host = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_HOST;

	    /* What port are we talking to? */
	    } else if (!strcmp(argv[arg], "-p")) {
		port = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_PORT;

	    /* What logprefix file should we use */
	    } else if (!strcmp(argv[arg], "-l")) {
		logprefix = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG;

	    /* Other options for HTTP/1.1 */
	    } else if (!strcmp(argv[arg], "-o")) {
		options = (arg+1 < argc) ? argv[++arg] : NULL;
		
	    /* What tcpdump program should we use */
	    } else if (!strcmp(argv[arg], "-t")) {
		tcpdump = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : TCPDUMP;
		
	    /* Should we produce binary tcpdump output? */
	    } else if (!strcmp(argv[arg], "-b")) {
		binary = YES;
		
	    /* What robot base path should we use */
	    } else if (!strcmp(argv[arg], "-r")) {
		robotpath = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : ROBOT_PATH;
		
	    /* What delay should we use betwen test runs */
	    } else if (!strcmp(argv[arg], "-d")) {
		zzzz = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_SLEEP;
		
	    /* What tests to run */
	    } else if (!strcmp(argv[arg], "-w")) {
		mask = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : HTTPALL;
		
	    } else {
		fprintf(stderr, "Don't understand `%s\'\n", argv[arg]);
		HelpScreen(*argv);
		return 1;
	    }
	} else {
	    fprintf(stderr, "Don't understand `%s\'\n", argv[arg]);
	    HelpScreen(*argv);
	    return 1;
	}
    }

    /* Initialize variables */
    suffix = binary ? BIN_SUFFIX : TEXT_SUFFIX;
    

    /* Run test for HTTP/1.0 */
    if (mask & HTTP10) {
	int cnt = 0;
	sprintf(program, "%s/%s/%s", robotpath, ROBOT_HTTP10, ROBOT);
	sprintf(logdir, "%s-http-10", logprefix);
	sprintf(url, "'http://%s:%s%s'", host, port, DEFAULT_HOME);
	CreateLogDir(logdir);
	for (cnt=1; cnt<6; cnt++) {
	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "ft", cnt, suffix); 
	    if (RunTest(program, url, host, port,
			"-n -saveimg",
			logpath, binary)) exit (-5);
	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "cv", cnt, suffix); 
	    if (RunTest(program, url, host, port,
			"-n -img",
			logpath, binary)) exit (-5);
	}
    }

    /* Run test for HTTP/1.1 w/o pipelining */
    if (mask & HTTP11) {
	int cnt = 0;
	sprintf(program, "%s/%s/%s", robotpath, ROBOT_HTTP11, ROBOT);
	sprintf(logdir, "%s-http-11", logprefix);
	sprintf(url, "'http://%s:%s%s'", host, port, DEFAULT_HOME);
	CreateLogDir(logdir);
	for (cnt=1; cnt<6; cnt++) {
	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "ft", cnt, suffix); 
	    if (RunTest(program, url, host, port,
			"-saveimg -nopipe -cache -flush -n -q",
			logpath, binary)) exit (-5);
	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "cv", cnt, suffix); 
	    if (RunTest(program, url, host, port,
			"-saveimg -nopipe -cache -validate -n -q",
			logpath, binary)) exit (-5);
	}
    }

    /* Run test for HTTP/1.1 with pipelining */
    if (mask & HTTP11PL) {
	int cnt = 0;
	sprintf(program, "%s/%s/%s", robotpath, ROBOT_HTTP11PL, ROBOT);
	sprintf(logdir, "%s-http-11-pl", logprefix);
	sprintf(url, "'http://%s:%s%s'", host, port, DEFAULT_HOME);
	CreateLogDir(logdir);
	for (cnt=1; cnt<6; cnt++) {
	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "ft", cnt, suffix); 
	    sprintf(param, "-saveimg -cache -flush -n -q %s", options ? options : "");
	    if (RunTest(program, url, host, port,
			param, logpath, binary)) exit (-5);

	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "cv", cnt, suffix);
	    sprintf(param, "-saveimg -cache -validate -n -q %s", options ? options : "");
	    if (RunTest(program, url, host, port,
			param, logpath, binary)) exit (-5);
	}
    }

    /* Run test for HTTP/1.1 with pipelining and compression */
    if (mask & HTTP11PLZIP) {
	int cnt = 0;
	sprintf(program, "%s/%s/%s", robotpath, ROBOT_HTTP11PL, ROBOT);
	sprintf(logdir, "%s-http-11-pl-zip", logprefix);
	sprintf(url, "'http://%s:%s%s'", host, port, ZIP_HOME);
	CreateLogDir(logdir);
	for (cnt=1; cnt<6; cnt++) {
	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "ft", cnt, suffix); 
	    sprintf(param, "-saveimg -cache -flush -n -q %s", options ? options : "");
	    if (RunTest(program, url, host, port,
			param, logpath, binary)) exit (-5);

	    sprintf(logpath, "%s/run-%s-%d.%s", logdir, "cv", cnt, suffix);
	    sprintf(param, "-saveimg -cache -validate -n -q %s", options ? options : "");
	    if (RunTest(program, url, host, port,
			param, logpath, binary)) exit (-5);
	}
    }

    return 0;
}

