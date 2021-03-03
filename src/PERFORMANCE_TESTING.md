# Performance Testing

## Docker Setup
```
docker run -d --rm --name redis -p 172.17.0.1:6379:6379 redis
docker run -d --rm --name uredis -v /tmp/sock:/run/redis fullaxx/uredis
```

## Running Tests
PING Test
```
./ping_test.dbg -H 172.17.0.1 -P 6379
./ping_test.dbg -S /tmp/sock/redis.sock
```

INCR Test
```
./incr_test.dbg -H 172.17.0.1 -P 6379
./incr_test.dbg -S /tmp/sock/redis.sock
```

SETEX Test
```
./setex_test.dbg -H 172.17.0.1 -P 6379
./setex_test.dbg -S /tmp/sock/redis.sock
```

LPUSH/LTRIM Test
```
./lpush_test.dbg -H 172.17.0.1 -P 6379
./lpush_test.dbg -S /tmp/sock/redis.sock
```

HSET Test
```
./hset_test.dbg -H 172.17.0.1 -P 6379
./hset_test.dbg -S /tmp/sock/redis.sock
```

LPUSH/LTRIM MULTI Test
```
./lpush_ltrim_multi_test.dbg -H 172.17.0.1 -P 6379
./lpush_ltrim_multi_test.dbg -S /tmp/sock/redis.sock
```
