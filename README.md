###  Azure Kinect a la Luna (AKALL)

###### Install Nvidia Docker toolkit on Ubuntu 18.04 or 20.04
Set distribution variable based on your system's configuration.
Note: only viable on systems equipped with Nvidia VGAs. Tested on Nvidia GTX-1080
```
distribution=ubuntu18.04
```
```
distribution=ubuntu20.04
```
```
curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | sudo apt-key add -
curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.list | sudo tee /etc/apt/sources.list.d/nvidia-docker.list

sudo apt-get update && sudo apt-get install -y nvidia-container-toolkit
sudo systemctl restart docker
```
source: [nvidia-container-toolkit](https://github.com/NVIDIA/nvidia-docker/issues/1186)

###### Build payload's Docker image
```
cd LUNA-R2D2/
docker build -t payload_image .
```
* This docker image will install the following key packages:
 * python3 and python3-pip
 * k4a-tools and libk4a1.4-dev
 * software-properties-common, curl, build-essential...etc
 * Compiles our custom Azure Kinect software
 * Creates and binds UNIX sockets in /tmp/payload_sockets

###### Run payload's Docker container
To run the payload's container execute this script on the host machine after a successful docker image build. It includes a *docker run* command that mounts 2 essential volumes onto the host machine, one to handle the payload's UNIX sockets bi-directional communications and the other for storage. In other words, using docker volume mounts (--volume, -v) enable the host machine to essentially develop a *tunnel of bits* between the enclosed docker container and itself. The third optional volume mount is the X11-UNIX socket that is only needed to test the *k4aviewer* tool, and was only used during the development of this project. This script is concluded with the execution of *entrypoint.sh* that contains application specific code used to start the Python3 UNIX socket server on the container and an optional shell for debugging and testing.

Note: passing extra hardware permissions such as for the VGA, and for the USB (--gpus all -- privileged) also seems to be mandatory to run the Azure Kinect camera from within the container. Privileged mode is also only needed in the development environment and is replaced with a specific USB port. [Read more](https://docs.docker.com/engine/reference/commandline/run/) about *docker run*

```
sudo chmod +x ./scripts/launch_container.sh
sudo ./scripts/launch_container.sh payload_container payload_image
```

###### Configuration on dev host machine:
Note: Only needed to test k4aviewer on the docker container.
```
xhost +local:docker
```
