#!/bin/bash

set -e

CFLAGS="-Wall ${GCCFLAGS}"
OPTCFLAGS="${CFLAGS} -O2"
DBGCFLAGS="${CFLAGS} -ggdb3 -DDEBUG"
#DBGCFLAGS+=" -DCHRONOMETRY -DTIME_NUM_CONVERSION"
#DBGCFLAGS+=" -DCHRONOMETRY -DTIME_REDIS_UPDATE"

rm -f *.dbg *.exe

gcc ${OPTCFLAGS} -DPING_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o ping_test.exe

gcc ${DBGCFLAGS} -DPING_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o ping_test.dbg

gcc ${OPTCFLAGS} -DINCR_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o incr_test.exe

gcc ${DBGCFLAGS} -DINCR_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o incr_test.dbg

gcc ${OPTCFLAGS} -DSETEX_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o setex_test.exe

gcc ${DBGCFLAGS} -DSETEX_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o setex_test.dbg

gcc ${OPTCFLAGS} -DSADD_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o sadd_test.exe

gcc ${DBGCFLAGS} -DSADD_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o sadd_test.dbg

gcc ${OPTCFLAGS} -DLPUSH_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o lpush_test.exe

gcc ${DBGCFLAGS} -DLPUSH_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o lpush_test.dbg

gcc ${OPTCFLAGS} -DHSET_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o hset_test.exe

gcc ${DBGCFLAGS} -DHSET_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o hset_test.dbg

gcc ${OPTCFLAGS} -DLPUSH_LTRIM_MULTI_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o lpush_ltrim_multi_test.exe

gcc ${DBGCFLAGS} -DLPUSH_LTRIM_MULTI_TEST test.c \
redisops.c getopts.c \
-lpthread -lhiredis -o lpush_ltrim_multi_test.dbg

strip *.exe
