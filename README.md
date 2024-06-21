##  Azure Kinect a la Luna (AKALL)
This custom software application utilizes the [Azure Kinect SDK](https://learn.microsoft.com/en-us/azure/kinect-dk/) through the C and C++ Sensor API to control the camera and capture data from its various sensors. The application is designed within a Docker container running Ubuntu 18.04 LTS, providing an isolated and portable environment for it to operate in, ideal for integration within larger computing systems such as rovers and robots.

## Features

- **UNIX Socket Server**: Implemented using Python3, the UNIX socket server enables communication with other programs and devices using the UNIX socket protocol. The server allows the application to control the camera using special capture sequence messages, which are custom commands or instructions.

- **Message Binding**: 
  - `pl_sock` binds messages incoming from the host machine to the payload's container.
  - `sm_sock` binds messages incoming to the host machine from the payload's container.

- **Shared Directory**: The shared directory `/storage` facilitates communication between the payload's container and the host machine. A successful capture generates four files: `color.jpg`, `depth16`, `ir16`, and `calibration.json`. These files are then compressed and stored in the shared directory.

- **Entrypoint Script**: The application's socket server and logger are launched using the script `./scripts/entrypoint.sh`. This script enables communication with other programs and devices using the UNIX socket protocol and allows for control of the camera using custom capture sequence messages.

### Capture Sequence:
Here is a list of example commands that can be used to affect the camera's built-in parameters and set compression and other parameters:

  * K15MJPG07201-1671006611
  * K30MJPG10802-1671006622
  * K15MJPG14403-1671006633
  * K30MJPG21602-1671006644
  * K15MJPG30720-1671006655

In these commands, the first part is mandatory to ensure a successful capture sequence ("K05MJPG07201", "K05MJPG10801", etc.), and it indicates the frame rate (FPS) and compression used, as well as other parameters. For instance, the "05" in the first part represents the FPS, "MJPG" represents the compression used, and "0720" is the resolution (in this case, 720p). The "1" at the end of the first part indicates the depth mode. The camera is capable of capturing up to 4k resolution, and there are four different depth modes available to access through the Azure Kinect SDK.

```
  K |  15  | MJPG | 0720 | 1          | 1671006655
  K |  FPS | COMP | RESO | DEPTH_MODE | TIMESTAMP
```
```
  FPS: 05, 15, 30
  COMP: MJPG (Primary), NV12, YUY2, BGRA, DP16, IR16
  RESO: 0720, 1080, 1440, 1536, 2160, 3072
  DEPTH_MODE: 0:OFF, 1:NFOV_2X2B 2:NFOV_U 3:WFOV_2X2B 4:WFOV_U 5:P_IR
```

The second and optional part ("EA-B128-C5-S32-H2-G0-WA-P0-L2-1671006611", etc.) specifies the Exposure, Contrast, Sharpness, Gain, White Balance, Black Light Compensation, Power Line Frequency, and timestamp. The values for these parameters may vary depending on the specific requirements of the application. For example, the "EA" in the second part sets the exposure to automatic mode, if a number is specified, such as "B128" the Contrast option is set to 128, "S" indicates the Sharpness, "G" indicates the Gain, "W" indicates the White Balance, and "1671006611" is the timestamp.

* Example capture sequence with color format:
  * K05MJPG07201-EA-B128-C5-S32-H2-G0-WA-P0-L2-1671006611
  * K05MJPG10801-EM1-B100-C2-S62-H4-G10-W3611-P1-L1-1671006622
  * K15MJPG14403-E80000-B60-C2-S40-H0-G128-W53611-P0-L1-1671006622
  * K30MJPG21602-E130000-B189-C3-S12-H1-G255-WA-P0-L2-1671006633
  * K15MJPG30720-EM11-B255-C8-S33-H3-G0-WA-P1-L1-1671006644

```
EA      |   B128   |   C5   |    S32   |    H2   | G0 |    WA   |      P0       |      L2     
EXPOSURE|BRIGHTNESS|CONTRAST|SATURATION|SHARPNESS|GAIN|WHITE BAL|BLACKLIGHT COMP|PWR LINE FREQ
```  
```
EXPOSURE TIME: A, M1-M13, M500-M130000. Default: EA (AUTO)
BRIGHTNESS: Integer from 0 to 255. Default: B128
CONTRAST: Integer from 0 to 10. Default: C5
SATURATION: Integer from 0 to 63. Default: S32
SHARPNESS: Integer from 0 to 4. Default: H2
GAIN: Integer from 0 to 255. Default: G0
WHITE BALANCE: A, Integer of 10 incremends from 2500 to 12500. Default: WA
BLACKLIGHT COMPENSATION: 0,1. Default: P0
POWER LINE FREQUENCY: 1: 50hz ,2: 60Hz. Default: L2
```

### Install and Build:
#### Install Nvidia Docker toolkit on Ubuntu 18.04 or 20.04:
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

#### Build payload's Docker image:
Execute the following commands on the host machine.
```
cd AKALL/
sudo docker build -t payload_image .
```
* This docker image will install the following key packages:
 * python3 and python3-pip
 * k4a-tools and libk4a1.4-dev
 * software-properties-common, curl, build-essential...etc
 * Compiles our custom Azure Kinect software
 * Creates and binds UNIX sockets in /tmp/payload_sockets

#### Launch container:
Similarly, from the host machine, execute the following:
```
sudo chmod +x ./scripts/launch_container.sh
sudo ./scripts/launch_container.sh payload_container akall_image
```
### Applications
#### Run interactive mode:
A python script, executed from the host machine, with user inputs to generate capture sequence.
```
cd include/
sudo python3 socket_coms_main.py
```

###### Example response:
```
Enter K4A's FPS (0:05 FPS 1:15 FPS 2:30 FPS) # 0
Enter K4A's Color Format (0:MJPG 1:NV12 2:YUY2 3:BGRA 4:DP16 5:IR16) # 0
Enter K4A's Color Resolution (0: Off 1:720 2:1080 3:1440 4:1536 5:2160 6:3072) # 1
Enter K4A's Depth Mode (0: Off 1:NFOV_2X2B 2:NFOV_U 3:WFOV_2X2B 4:WFOV_U 5:P_IR) # 1
Set K4A's Color Settings (y/n) #y
Enter Exposure Time (A, M1-M13, M500-M130000) d:A #A
Enter Brightness (0-255) d:128 #123
Enter Contrast (0-10) d:5 #5
Enter Saturation (0-63) d:32 #32
Enter Sharpness (0-4) d:2 #2
Enter Gain (0-255) d:0 #0
Enter White Balance (A, 2500-12500) d:A #A
Enter Backlight Compensation (0,1) d:0 #0
Enter Power Line Frequency (1: 50hz ,2: 60Hz) d:2 #2
Settings Recorded..

Capturing K4A Images with the following settings:
K4A_FRAMES_PER_SECOND_5
K4A_IMAGE_FORMAT_COLOR_MJPG
K4A_COLOR_RESOLUTION_720P
K4A_DEPTH_MODE_NFOV_2X2BINNED
CS EA-B123-C5-S32-H2-G0-WA-P0-L2
```
#### Run capture sequence console:
A python script, executed form the host machine, that launches the debugging console to send and receive data from the container. This scripts listens and binds to the ``sm_sock`` UNIX domain socket. The filename is sent through that socket upon a successful capture. 
**Note that this script will automatically append a timestamp as a last parameter in the capture sequence.**
```
cd include/
sudo python3 socket_coms_console.py
```
###### Example responses:
```
Enter Capture Sequence # K15MJPG30721

[HOST MACHINE] K4A Image str: K15MJPG30721-1673315086
[HOST MACHINE] Sending data on socket: /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] connecting to /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] Connected
[HOST MACHINE] Done..

[HOST MACHINE]  Received data from /tmp/payload_sockets/kinect_luna/sm_sock
                DATA [17]: b'1675132016.tar.gz'

Enter Capture Sequence # K30MJPG21602-EA-B128-C5-S32-H1-G0-WA-P0-L2

[HOST MACHINE] K4A Image str: K30MJPG21602-EA-B128-C5-S32-H1-G0-WA-P0-L2-1673315140
[HOST MACHINE] Sending data on socket: /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] connecting to /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] Connected
[HOST MACHINE] Done..

[HOST MACHINE]  Received data from /tmp/payload_sockets/kinect_luna/sm_sock
                DATA [17]: b'1675132036.tar.gz'
```

#### Storage management commands

* Empty /storage directory: **SM-RM-ALL**
* Remove single capture using filename (timestamp): **SM-RM-FILENAME**
  * Please note that ".tar.gz" is automatically appended to the filename
  * The files are stored in a shared directory "/tmp/payload_storage" on the host machine
  * The files are automatically named by the software running within the docker container, the format used to name these files is as follows: timestamp.tar.gz example: 1673370956.tar.gz

###### Example response:
```
Enter Capture Sequence # SM-RM-1673370956

[HOST MACHINE] K4A Image str: SM-RM-1673370956-1673371013
[HOST MACHINE] Sending data on socket: /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] connecting to /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] Connected
[HOST MACHINE] Done..

Enter Capture Sequence # SM-LS-ALL

[HOST MACHINE] K4A Capture cmd: SM-LS-ALL-1673383032
[HOST MACHINE] Sending data on socket: /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] connecting to /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] Connected
[HOST MACHINE] Done..

[HOST MACHINE] Received data from /tmp/payload_sockets/kinect_luna/sm_sock
 DATA [35]: b'1673382998.tar.gz,1673383011.tar.gz'

Enter Capture Sequence # SM-RM-ALL

[HOST MACHINE] K4A Image str: SM-RM-ALL-1673371045
[HOST MACHINE] Sending data on socket: /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] connecting to /tmp/payload_sockets/kinect_luna/pl_sock
[HOST MACHINE] Connected
[HOST MACHINE] Done..
```

#### Run test script
The following test script generates **108 capture sequences** covering some of the basic parameters of the Azure Kinect sensor API such as FPS, resolution, and depth mode.

```
cd include/
sudo python3 socket_coms_test.py
```

#### Debugging
To access the running docker container for debugging purposes issue the following docker command:
```
docker run --rm -it --entrypoint /bin/bash akall_image
```

#### Convert Depth and IR data into grayscale Image
Each pixel of DEPTH16 (and IR16) data is two bytes of little-endian unsigned depth data (b16g). We can use ImageMagick to convert the depth data into a grayscale .pgm or .bmp image:

Install ImageMagick using apt:
```
sudo apt update
sudo apt install imagemagick
```

```
convert -size 640x576 -depth 16 -endian LSB -define quantum:format=unsigned -define quantum:separate -depth 16 gray:IR16_FILENAME -normalize IR16_CONVERTED_FILENAME.pgm
```
The table below can help you find the correct resolution associated with each depth mode in the capture sequence. Example: K05MJPG07201 is a capture sequence message where the depth mode is set to 1 (NFOV 2x2 binned with a resolution of 320x288). [Read more: Depth camera operating modes](https://learn.microsoft.com/en-us/azure/kinect-dk/hardware-specification)

| AKAL ID |         MODE         |    RES    |    FOI    |     RANGE     | EXPOSURE |
|:-------:|:--------------------:|:---------:|:---------:|:-------------:|:--------:|
| 0       | OFF                  | N/A       | N/A       | N/A           | N/A      |
| 1       | NFOV 2x2 binned (SW) | 320x288   | 75°x65°   | 0.5 - 5.46 m  | 12.8 ms  |
| 2       | NFOV unbinned        | 640x576   | 75°x65°   | 0.5 - 3.86 m  | 12.8 ms  |
| 3       | WFOV 2x2 binned      | 512x512   | 120°x120° | 0.25 - 2.88 m | 12.8 ms  |
| 4       | WFOV unbinned        | 1024x1024 | 120°x120° | 0.25 - 2.21 m | 20.3 ms  |
| 5       | Passive IR           | 1024x1024 | N/A       | N/A           | 1.6 ms   |
