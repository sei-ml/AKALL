#!/bin/sh

# Expose the X server on the host, only needed on dev machine.
sudo xhost +local:root

export PAYLOAD_STORAGE=/tmp/payload_storage

export PAYLOAD_SOCKETS=/tmp/payload_sockets/kinect_luna

export ENTRYPOINT=/home/payload/workspace/scripts/entrypoint.sh

sudo docker run -it --rm \
    \
    --gpus all --privileged \
    -e DISPLAY=$DISPLAY \
    -v ${PAYLOAD_SOCKETS}:/tmp/payload_sockets/ \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v ${PAYLOAD_STORAGE}:/storage \
    -w /home/payload/ \
    \
    --name=$1 \
    $2 \
    ${ENTRYPOINT}
