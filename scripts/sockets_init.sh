#!/bin/sh

mkdir /tmp/payload_sockets
nc -vklU /tmp/payload_sockets/pl_sock &
PID=$!
sleep 1
kill $PID
