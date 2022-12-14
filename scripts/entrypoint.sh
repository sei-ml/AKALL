#!/bin/sh

touch /output.log
python3 /home/payload/workspace/include/socket_coms_local.py &>> /output.log &

#Used for debugging
bash
