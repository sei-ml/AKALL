#!/bin/sh

mkdir /tmp/payload_sockets
nc -vklU /tmp/payload_sockets/pl_sock &
PID=$!
sleep 1
kill $PID

touch /output.log
python3 /home/payload/workspace/include/socket_coms_local.py &>> /output.log &

#Used for debugging
bash
