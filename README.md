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
cd AKALL/
docker build -t payload_image .
```
* This docker image will install the following key packages:
 * python3 and python3-pip
 * k4a-tools and libk4a1.4-dev
 * software-properties-common, curl, build-essential...etc
 * Compiles our custom Azure Kinect software
 * Creates and binds UNIX sockets in /tmp/payload_sockets

###### Concept of Operation
* [pl_sock] binds messages incoming from the host enclave to the payload's container.
* [/storage] shared directory within the payload's container  
* [entrypoint.sh] launches application's socket server and logger
* Example socket messages:
  * K15MJPG07201-1671006611
  * K30MJPG10802-1671006622
  * K15MJPG11403-1671006633
  * K30MJPG21604-1671006644
  * K15MJPG30720-1671006655

###### Launch Container
```
sudo chmod +x ./scripts/launch_container.sh
sudo ./scripts/launch_container.sh payload_container payload_image
```
