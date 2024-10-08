#ifndef __REDIS_OPERATIONS__
#define __REDIS_OPERATIONS__

int do_lpush_ltrim_multi(char *, unsigned long, unsigned long);
int do_hset(char *, unsigned long, unsigned long);
//int do_ltrim(char *, unsigned long);
int do_lpush(char *, unsigned long, unsigned long);
int do_sadd(unsigned long key, unsigned long val);
int do_setex(unsigned long, unsigned long);
int do_incr(char *);
int do_ping(void);
int do_connect(char *, unsigned short);
void do_disconnect(void);

#endif
