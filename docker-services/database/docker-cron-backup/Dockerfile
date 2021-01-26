FROM alpine:3.12
# FROM mysql:5.7
# FROM leafney/docker-alpine-mysql:latest

LABEL maintainer "Fco. Javier Delgado del Hoyo <frandelhoyo@gmail.com>"

RUN apk add --update tzdata bash mysql-client gzip openssl mariadb-connector-c && rm -rf /var/cache/apk/*

ARG OS=alpine-linux
ARG ARCH=amd64
ARG DOCKERIZE_VERSION=v0.6.1

RUN wget https://github.com/jwilder/dockerize/releases/download/$DOCKERIZE_VERSION/dockerize-$OS-$ARCH-$DOCKERIZE_VERSION.tar.gz \
    && tar -C /usr/local/bin -xzvf dockerize-$OS-$ARCH-$DOCKERIZE_VERSION.tar.gz \
    && rm dockerize-$OS-$ARCH-$DOCKERIZE_VERSION.tar.gz

# RUN apt update && apt install -y libmysqlclient-dev libssl-dev libace-6.4.5 libace-dev libreadline-dev net-tools tzdata;


ENV CRON_TIME="0 0 * * *" \
    MYSQL_HOST="51.161.33.67" \
    MYSQL_PORT="3306" \
    TIMEOUT="500s"

COPY ["run.sh", "backup.sh", "restore.sh", "/"]
RUN mkdir /backup && chmod u+x /backup.sh /restore.sh
VOLUME ["/backup"]

CMD dockerize -wait tcp://${MYSQL_HOST}:${MYSQL_PORT} -timeout 500s /run.sh

# HEALTHCHECK --interval=5s --timeout=15s --start-period=30s --retries=3 CMD mysqladmin -uroot -p$MYSQL_ROOT_PASSWORD ping -h localhost
 

# FROM alpine:latest

# COPY ./scripts/daily/* /etc/periodic/daily

# RUN apk update && \
#     apk upgrade && \
#     # apk add --no-cache libmysqlclient-dev && \
#     chmod a+x /etc/periodic/daily/*
