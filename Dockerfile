# ------------------------------------------------------------------------------
# Pull base image
FROM redis:latest
LABEL author="Brett Kuskie <fullaxx@gmail.com>"

# ------------------------------------------------------------------------------
# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# ------------------------------------------------------------------------------
# Add app.sh and redis.conf
COPY app.sh /app/app.sh
COPY redis.conf /usr/local/etc/redis/redis.conf

# ------------------------------------------------------------------------------
# Add volumes
VOLUME /data
VOLUME /run/redis

# ------------------------------------------------------------------------------
# Define Healthcheck
HEALTHCHECK --start-period=2s --interval=5s \
CMD redis-cli -s /run/redis/redis.sock PING

# ------------------------------------------------------------------------------
# Define default command
# CMD [ "redis-server", "/usr/local/etc/redis/redis.conf" ]
CMD [ "/app/app.sh" ]
