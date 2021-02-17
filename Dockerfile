# ------------------------------------------------------------------------------
# Pull base image
FROM redis:latest
MAINTAINER Brett Kuskie <fullaxx@gmail.com>

# ------------------------------------------------------------------------------
# Set environment variables
ENV DEBIAN_FRONTEND noninteractive

# ------------------------------------------------------------------------------
# Add app.sh and redis.conf
COPY app.sh /app/app.sh
COPY redis.conf /usr/local/etc/redis/redis.conf

# ------------------------------------------------------------------------------
# Add volumes
VOLUME /data
VOLUME /run/redis

# ------------------------------------------------------------------------------
# Define default command
# CMD [ "redis-server", "/usr/local/etc/redis/redis.conf" ]
CMD [ "/app/app.sh" ]
