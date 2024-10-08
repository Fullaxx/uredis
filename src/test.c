#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "getopts.h"
#include "redisops.h"

#ifdef CHRONOMETRY
#include "chronometry.h"
#endif

//static void parse_env(void);
static void parse_args(int argc, char **argv);

int g_shutdown = 0;
char *g_rsock = NULL;
char *g_rhost = NULL;
unsigned short g_rport = 0;
unsigned int g_threads = 0;

int g_print_stats = 1;
unsigned long g_counter = 0;
static void alarm_handler(int signum)
{
	unsigned long l_count = g_counter;
	g_counter = 0;
	if(g_print_stats) {
		printf("%9lu\n", l_count);
		fflush(stdout);
	}
	(void) alarm(1);
}

static void shutdown_message(char *what)
{
	g_shutdown = 1;
	usleep(10000);
	fprintf(stderr, "%s failure!\n", what);
	exit(1);
}

static void sig_handler(int signum)
{
	switch(signum) {
		case SIGPIPE:
			fprintf(stderr, "SIGPIPE\n");
		case SIGHUP:
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			g_shutdown = 1;
			break;
	}
}

#ifdef LPUSH_LTRIM_MULTI_TEST
static void* run_test(void *p)
{
	int r;
	unsigned long val = 0;
	while(!g_shutdown) {
		r = do_lpush_ltrim_multi("TESTLIST", val++, 900000);
		if(r) { fprintf(stderr, "LPUSH_LTRIM_MULTI ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

#ifdef HSET_TEST
static void* run_test(void *p)
{
	int r;
	unsigned long key = 0;
	unsigned long val = 0;
	while(!g_shutdown) {
		r = do_hset("TESTHASH", key++, val++);
		if(r) { fprintf(stderr, "HSET ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

#ifdef LPUSH_TEST
static void* run_test(void *p)
{
	int r;
	unsigned long val = 0;
	while(!g_shutdown) {
		r = do_lpush("TESTLIST", val++, 900000);
		if(r) { fprintf(stderr, "LPUSH ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

#ifdef SADD_TEST
static void* run_test(void *p)
{
	int r;
	unsigned long key = 0;
	while(!g_shutdown) {
		r = do_sadd(key++, 0);
		if(r) { fprintf(stderr, "SET ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

#ifdef SETEX_TEST
static void* run_test(void *p)
{
	int r;
	unsigned long key = 0;
	while(!g_shutdown) {
		r = do_setex(key++, 0);
		if(r) { fprintf(stderr, "SET ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

#ifdef INCR_TEST
static void* run_test(void *p)
{
	int r;
	while(!g_shutdown) {
		r = do_incr("TESTKEY");
		if(r) { fprintf(stderr, "INCR ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

#ifdef PING_TEST
static void* run_test(void *p)
{
	int r;
	while(!g_shutdown) {
		r = do_ping();
		if(r) { fprintf(stderr, "PING ERROR!\n"); g_shutdown=1; }
		g_counter++;
	}
	return NULL;
}
#endif

static int start_threads(unsigned int tcount)
{
	int err;
	pthread_attr_t attr;
	pthread_t thr_id;
	char thr_name[16];

	err = pthread_attr_init(&attr);
	if(err) { perror("pthread_attr_init()"); return -1; }

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(err) { perror("pthread_attr_setdetachstate()"); return -2; }

	while(tcount-- > 0) {
		err = pthread_create(&thr_id, &attr, &run_test, NULL);
		if(err) { perror("pthread_create()"); return -3; }
		snprintf(thr_name, sizeof(thr_name), "run_test_%06d", tcount+1);
		(void)pthread_setname_np(thr_id, thr_name);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int err;

	//parse_env();
	parse_args(argc, argv);

	if(g_rport > 0) {
		err = do_connect(g_rhost, g_rport);
	} else {
		err = do_connect(g_rsock, 0);
	}
	if(err) { shutdown_message("do_connect()"); }

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	signal(SIGPIPE,	sig_handler);
	signal(SIGALRM, alarm_handler);
	(void) alarm(1);

	if(g_threads == 0) {
		(void)run_test(NULL);
	} else {
		err = start_threads(g_threads);
		if(err) { shutdown_message("start_threads()"); }
		while(!g_shutdown) { usleep(100); }
		usleep(10000);
	}

	do_disconnect();
	if(g_rsock) { free(g_rsock); }
	if(g_rhost) { free(g_rhost); }
	return 0;
}

// We will parse ENV first
// parse_args will override
/*
static void parse_env(void)
{
	char *key;

	key = "LISTS";
	if(getenv(key)) { set_list_count(atoi(getenv(key))); }

	key = "LSIZE";
	if(getenv(key)) { set_list_size(atoi(getenv(key))); }

	key = "CORES";
	if(getenv(key)) { g_chaos_threads = atoi(getenv(key)); }

	key = "CHAOS";
	if(getenv(key)) { g_chaos_amt = atoi(getenv(key)); }

	key = "SAVEACORE";
	if(getenv(key)) { g_saveacore = 1; }
}
*/

struct options opts[] = 
{
	{ 1, "rsock",	"the Redis Socket to connect to",			"S",	1 },
	{ 2, "rhost",	"the Redis Hostname to connect to",			"H",	1 },
	{ 3, "rport",	"the Redis Port number",					"P",	1 },
	{ 4, "threads",	"the number of threads to spawn",			"t",	1 },
	{ 0, NULL,		NULL,										NULL,	0 }
};

static void parse_args(int argc, char **argv)
{
	int c;
	char *args;

	while ((c = getopts(argc, argv, opts, &args)) != 0) {
		switch(c) {
			case -2:
				// Special Case: Recognize options that we didn't set above.
				fprintf(stderr, "Unknown Getopts Option: %s\n", args);
				break;
			case -1:
				// Special Case: getopts() can't allocate memory.
				fprintf(stderr, "Unable to allocate memory for getopts().\n");
				exit(EXIT_FAILURE);
				break;
			case 1:
				g_rsock = strdup(args);
				break;
			case 2:
				g_rhost = strdup(args);
				break;
			case 3:
				g_rport = atoi(args);
				break;
			case 4:
				g_threads = atoi(args);
				break;
			default:
				fprintf(stderr, "Unexpected getopts Error! (%d)\n", c);
				break;
		}

		//This free() is required since getopts() automagically allocates space for "args" everytime it's called.
		free(args);
	}

	if(!g_rsock && !g_rhost) {
		fprintf(stderr, "I need to connect to redis! (Fix with -S/-H)\n");
		exit(1);
	}

	if(g_rsock && g_rhost) {
		fprintf(stderr, "I need either a file socket or hostname! (Fix with -S/-H)\n");
		exit(1);
	}

	if(g_rhost && (g_rport == 0)) {
		fprintf(stderr, "I need a port number! (Fix with -P)\n");
		exit(1);
	}

	if((g_rport > 0) && (!g_rhost)) {
		fprintf(stderr, "I need a hostname to accompany this port number! (Fix with -H)\n");
		exit(1);
	}
}
