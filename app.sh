#!/bin/bash

# if [ ! -d /run/redis ]; then mkdir /run/redis; fi
# chmod 0777 /run/redis
# chmod +t /run/redis

exec redis-server /usr/local/etc/redis/redis.conf
