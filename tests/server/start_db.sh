#!/bin/bash
# Starts a private MariaDB instance for testing (datadir /home/claude/testenv/db/data, port 3306, socket db/mysqld.sock)
D=/home/claude/testenv/db
mkdir -p $D/data
if [ ! -d $D/data/mysql ]; then
  mariadb-install-db --user=root --datadir=$D/data --auth-root-authentication-method=socket >$D/install.log 2>&1
fi
if ! mariadb-admin --socket=$D/mysqld.sock ping >/dev/null 2>&1; then
  nohup mariadbd --user=root --datadir=$D/data --socket=$D/mysqld.sock --port=3306 --bind-address=127.0.0.1 \
    --pid-file=$D/mysqld.pid --log-error=$D/error.log --character-set-server=utf8mb4 --collation-server=utf8mb4_unicode_ci >/dev/null 2>&1 &
  for i in $(seq 30); do mariadb-admin --socket=$D/mysqld.sock ping >/dev/null 2>&1 && break; sleep 1; done
fi
mariadb-admin --socket=$D/mysqld.sock ping
