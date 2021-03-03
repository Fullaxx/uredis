#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <stdint.h>
#include <pthread.h>
#include <hiredis/hiredis.h>

#ifdef CHRONOMETRY
#include "chronometry.h"
#endif

extern int g_shutdown;
extern unsigned int g_threads;
pthread_mutex_t rlock = PTHREAD_MUTEX_INITIALIZER;
redisContext *rc = NULL;

#include <errno.h>
static inline void handle_redis_error(void)
{
	char *etype = NULL;
	switch(rc->err) {
		case REDIS_ERR_IO:
			fprintf(stderr, "REDIS_ERR_IO: %s\n", strerror(errno));
			break;
		case REDIS_ERR_EOF:
			etype = "REDIS_ERR_EOF";
			break;
		case REDIS_ERR_PROTOCOL:
			etype = "REDIS_ERR_PROTOCOL";
			break;
		case REDIS_ERR_OOM:
			etype = "REDIS_ERR_OOM";
			break;
		case REDIS_ERR_OTHER:
			etype = "REDIS_ERR_OTHER";
			break;
		default:
			etype = "UNKNOWN";
	}
	if(etype) {
		fprintf(stderr, "%s: %s\n", etype, rc->errstr);
	}
	g_shutdown = 1;
}

int do_lpush_ltrim_multi(char *list, unsigned long val, unsigned long listsize)
{
	int retval = 0;
	redisReply *reply;

	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	freeReplyObject(redisCommand(rc, "MULTI"));
	freeReplyObject(redisCommand(rc, "LPUSH %s %lu", list, val));
	freeReplyObject(redisCommand(rc, "LTRIM %s 0 %u", list, listsize-1));
	reply = (redisCommand(rc, "EXEC"));
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }

	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	freeReplyObject(reply);
	return retval;
}

int do_hset(char *hash, unsigned long key, unsigned long val)
{
	int retval = 0;
	redisReply *reply;
	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	reply = redisCommand(rc, "HSET %s %lu %lu", hash, key, val);
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	//if(reply->type == REDIS_REPLY_STATUS) { printf("LTRIM: %s\n", reply->str); }
	freeReplyObject(reply);
	return retval;
}

int do_ltrim(char *list, unsigned long listsize)
{
	int retval = 0;
	redisReply *reply;
	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	reply = redisCommand(rc, "LTRIM %s 0 %u", list, listsize-1);
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	//if(reply->type == REDIS_REPLY_STATUS) { printf("LTRIM: %s\n", reply->str); }
	freeReplyObject(reply);
	return retval;
}

int do_lpush(char *list, unsigned long val, unsigned long listsize)
{
	unsigned long members;
	int retval = 0;
	redisReply *reply;
	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	reply = redisCommand(rc, "LPUSH %s %lu", list, val);
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	if(reply->type == REDIS_REPLY_INTEGER) {
		members = (uint64_t)reply->integer;
		//printf("LPUSH: %9lu\n", count);
		if(members > listsize) { do_ltrim(list, listsize); }
	}
	freeReplyObject(reply);
	return retval;
}

int do_setex(unsigned long key, unsigned long val)
{
	int retval = 0;
	redisReply *reply;
	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	reply = redisCommand(rc, "SET %lu %lu EX 2", key, val);
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	freeReplyObject(reply);
	return retval;
}

int do_incr(char *key)
{
	int retval = 0;
	redisReply *reply;
	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	reply = redisCommand(rc, "INCR %s", key);
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	freeReplyObject(reply);
	return retval;
}

int do_ping(void)
{
	int retval = 0;
	redisReply *reply;
	if(g_threads > 1) { pthread_mutex_lock(&rlock); }
	reply = redisCommand(rc, "PING");
	if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	if(!reply) { handle_redis_error(); return -1; }
	if(reply->type == REDIS_REPLY_ERROR) { retval = 1; }
	freeReplyObject(reply);
	return retval;
}

// return -1 means you have an open redis handle already (or it appears that way), bail
// return -2 means redisConnect() failed, bail
// return -3 means there was an connetion error during redisConnect()
int do_connect(char *dest, unsigned short port)
{
	if(rc) { return -1; }

	if(port) { rc = redisConnect(dest, port); }
	else	{ rc = redisConnectUnix(dest); }

	if(!rc) {
		fprintf(stderr, "Connection error: failed to allocate redis context\n");
		return -2;
	}

	if(rc->err) {
		fprintf(stderr, "Connection error: %s\n", rc->errstr);
		redisFree(rc);
		rc = NULL;
		return -3;
	}

	return 0;
}

void do_disconnect(void)
{
	if(rc) {
		if(g_threads > 1) { pthread_mutex_lock(&rlock); }
		redisFree(rc);
		rc = NULL;
		if(g_threads > 1) { pthread_mutex_unlock(&rlock); }
	}
}
