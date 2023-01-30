###
### To Build:
### docker build -t <payload_image_name> <path_to_folder_containing_Dockerfile>
###
### To Run:
### ./scripts/launch_container.sh <payload_container_name> <payload_image_name>
###

### Base image for your container
FROM nvidia/cuda:11.4.0-base-ubuntu18.04

### Install required packages/dependencies
RUN apt update -y && apt install -y \
    python3 \
    python3-pip \
    curl \
    software-properties-common \
    build-essential

### Update sources lists
RUN curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add - && \
    apt-add-repository https://packages.microsoft.com/ubuntu/18.04/multiarch/prod

### Install Azure Kinect SDK
RUN apt update -y && ACCEPT_EULA=y apt install -y k4a-tools && \
    apt install -y \
    libk4a1.4-dev

### Copy source code to container
COPY src /home/payload/workspace/src
COPY include /home/payload/workspace/include
COPY scripts /home/payload/workspace/scripts
RUN chmod +x /home/payload/workspace/scripts/*.sh

### build payload software
RUN gcc -g -Wall  /home/payload/workspace/src/k4a-capture.cpp -o /usr/local/bin/check-device -lk4a -lstdc++; \
gcc -g -Wall  /home/payload/workspace/src/get-calibration.cpp -o /usr/local/bin/calibrate -lk4a -lstdc++

# Env vars for the nvidia-container-runtime.
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute

### Set payload entrypoint
WORKDIR /home/payload/workspace
CMD ["/bin/bash", "/home/payload/workspace/scripts/entrypoint.sh"]

LABEL maintainer="Don Derek Haddad <ddh@mit.edu>, Dogi <stefan@unterhauser.name>"
LABEL version="1.1"
LABEL description="Docker image for Azure Kinect SDK with CUDA 11.4.0 on Ubuntu 18.04"
