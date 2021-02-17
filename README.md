# uredis
A redis server configured for unix socket connections

## Base Docker Image
[Redis](https://hub.docker.com/_/redis)

## Get the image from Docker Hub or build it yourself
```
docker pull fullaxx/uredis
docker build -t="fullaxx/uredis" github.com/Fullaxx/uredis
```

## Volume Options
Data: The location of your Redis rdb file
```
-v /srv/docker/redis/data:/data
```
Sock: The location of your Redis unix socket file
```
-v /srv/docker/redis/sock:/run/redis
```

## Launch uredis
Run the image exposing your rdb file and a unix socket for connecting
```
docker run -d \
-v /srv/docker/redis/data:/data \
-v /srv/docker/redis/sock:/run/redis \
fullaxx/uredis
```
