#!/usr/bin/python

### * ************************************ * ###
### *   Simple threaded Unix socket server * ###
### *   running within the host machine    * ###
### * ************************************ * ###

import socket
import os, os.path
import threading
import time

PRINT_PREPEND = '[HOST MACHINE] '
UNIX_SOCKETS_BASE_DIR = '/tmp/payload_sockets/kinect_luna/'
UNIX_ADDR_OUT = UNIX_SOCKETS_BASE_DIR + 'pl_sock'

class UNIX_Coms():

    def __init__(self, host_addr):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.server_address = host_addr

    '''
        Creates and binds a new UNIX socket
        Server (listener) side
        - Removes existing socket if it exists
    '''
    def bind_to_socket(self):

        if os.path.exists(self.server_address):
            os.remove(self.server_address)

        self.sock.bind(self.server_address)
        print(PRINT_PREPEND + 'starting up on {}'.format(self.server_address))

        self.unix_start()

    '''
        Start listening for incoming messages
        sever (listener) side
    '''
    def unix_start(self):
        threading.Thread(target=self.listen).start()

    '''
        Listen for incoming messages (called by unix_start)
    '''
    def listen(self):
        print(PRINT_PREPEND + 'Listening for connections on {}'.format(self.server_address))
        while (True):
            data = self.sock.recv(4096)
            if data:
                print(PRINT_PREPEND + 'Received data from {} | DATA [{}]: {}'.format(self.server_address, len(data), data))

    '''
        Connects to existing UNIX sockets
        client (sender) side
    '''
    def connect_to_socket(self):
        connect = False
        try:
            print(PRINT_PREPEND + 'connecting to {}'.format(self.server_address))
            self.sock.connect(self.server_address)
            connect = True
            print(PRINT_PREPEND + 'Connected')
        except socket.error as msg:
            print(msg)

    '''
        Send data to the server
        - connectsto the socket and sends the data
    '''
    def send(self, msg):
        print(PRINT_PREPEND + 'Sending data on socket: {}'.format(self.server_address))

        self.connect_to_socket()
        self.sock.sendall(msg)

    def close(self):
        self.sock.close()
        print(PRINT_PREPEND + 'Socket closed: {}'.format(self.server_address))

def main():

    unix_out = {}

    def closeAll():
        [unix_out[key].close() for key in unix_out]

    try:
        unix_out = {
            'OUT' : UNIX_Coms(UNIX_ADDR_OUT),
        }
    except:
        print(PRINT_PREPEND + 'Error creating outgoing UNIX socket(s)')
        unix_out = None

    time.sleep(1)

    input_error = "Invalid choice.. using default settings."
    camera_fps = "K4A_FRAMES_PER_SECOND_5"
    repeat = True

    while(repeat):
        input_0 = input("Enter K4A's FPS (0:05 FPS 1:15 FPS 2:30 FPS) # ")
        if input_0 == str(0):
            camera_fps = "K4A_FRAMES_PER_SECOND_5"
        elif input_0 == str(1):
            camera_fps = "K4A_FRAMES_PER_SECOND_15"
        elif input_0 == str(2):
            camera_fps = "K4A_FRAMES_PER_SECOND_30"
        else:
            input_0 = 0
            print(input_error)

        input_1 = input("Enter K4A's Color Format (0:MJPG 1:NV12 2:YUY2 3:BGRA 4:DP16 5:IR16) # ")
        color_format = "K4A_IMAGE_FORMAT_COLOR_MJPG"
        if input_1 == str(0):
            color_format = "K4A_IMAGE_FORMAT_COLOR_MJPG"
        elif input_1 == str(1):
            color_format = "K4A_IMAGE_FORMAT_COLOR_NV12"
        elif input_1 == str(2):
            color_format = "K4A_IMAGE_FORMAT_COLOR_YUY2"
        elif input_1 == str(3):
            color_format = "K4A_IMAGE_FORMAT_COLOR_BGRA32"
        elif input_1 == str(4):
            color_format = "K4A_IMAGE_FORMAT_DEPTH16"
        elif input_1 == str(5):
            color_format = "K4A_IMAGE_FORMAT_IR16"
        else:
            input_1 = 0
            print(input_error)

        input_2 = input("Enter K4A's Color Resolution (0: Off 1:720 2:1080 3:1440 4:1536 5:2160 6:3072) # ")
        color_resolution = "K4A_COLOR_RESOLUTION_720P"
        if input_2 == str(0):
            color_resolution = "K4A_COLOR_RESOLUTION_OFF"
        elif input_2 == str(1):
            color_resolution = "K4A_COLOR_RESOLUTION_720P"
        elif input_2 == str(2):
            color_resolution = "K4A_COLOR_RESOLUTION_1080P"
        elif input_2 == str(3):
            color_resolution = "K4A_COLOR_RESOLUTION_1440P"
        elif input_2 == str(4):
            color_resolution = "K4A_COLOR_RESOLUTION_1536P"
        elif input_2 == str(5):
            color_resolution = "K4A_COLOR_RESOLUTION_2160P"
        elif input_2 == str(6):
            color_resolution = "K4A_COLOR_RESOLUTION_3072P"
        else:
            input_2 = 1
            print(input_error)

        input_3 = input("Enter K4A's Depth Mode (0: Off 1:NFOV_2X2B 2:NFOV_U 3:WFOV_2X2B 4:WFOV_U 5:P_IR) # ")
        depth_mode = "K4A_DEPTH_MODE_NFOV_2X2BINNED"
        if input_3 == str(0):
            depth_mode = "K4A_DEPTH_MODE_OFF"
        elif input_3 == str(1):
            depth_mode = "K4A_DEPTH_MODE_NFOV_2X2BINNED"
        elif input_3 == str(2):
            depth_mode = "K4A_DEPTH_MODE_NFOV_UNBINNED"
        elif input_3 == str(3):
            depth_mode = "K4A_DEPTH_MODE_WFOV_2X2BINNED"
        elif input_3 == str(4):
            depth_mode = "K4A_DEPTH_MODE_WFOV_UNBINNED"
        elif input_3 == str(5):
            depth_mode = "K4A_DEPTH_MODE_PASSIVE_IR"
        else:
            input_3 = 1
            print(input_error)

        input_4 = input("Set K4A's Color Settings (y/n) #")
        color_settings = "D"

        if input_4 == "y" or input_4 == "Y":
            input_4_1 = input("Enter Exposure Time (A, M1-M13, M500-M130000) d:A #")
            input_4_2 = input("Enter Brightness (0-255) d:128 #")
            input_4_3 = input("Enter Contrast (0-10) d:5 #")
            input_4_4 = input("Enter Saturation (0-63) d:32 #")
            input_4_5 = input("Enter Sharpness (0-4) d:2 #")
            input_4_6 = input("Enter Gain (0-255) d:0 #")
            input_4_7 = input("Enter White Balance (A, 2500-12500) d:A #")
            input_4_8 = input("Enter Backlight Compensation (0,1) d:0 #")
            input_4_9 = input("Enter Power Line Compensation (1: 50hz ,2: 60Hz) d:2 #")

            color_settings = 'E'+input_4_1+"-"+'B'+input_4_2+"-"+'C'+input_4_3+"-"+'S'+input_4_4+"-"+'H'+input_4_5+"-"+'G'+input_4_6+"-"+'W'+input_4_7+"-"+'P'+input_4_8+"-"+'L'+input_4_9

        print("Settings Recorded..\n")
        print("Capturing K4A Images with the following settings:")

        print(camera_fps)
        print(color_format)
        print(color_resolution)
        print(depth_mode)
        print("CS " + color_settings)

        opt = [['05','15','30'], ['MJPG','NV12','YUY2','BGRA','DP16','IR16'], ['0000','0720','1080','1440','1536','2160','3072'], ['0','1','2','3','4','5']]

        cmd = 'K'+opt[0][int(input_0)] + opt[1][int(input_1)] + opt[2][int(input_2)] + opt[3][int(input_3)]

        print('\n'+PRINT_PREPEND + 'K4A Image str: ' + cmd)

        ts = int(time.time())
        cmd_byte = bytearray(cmd+'-'+color_settings+'-'+str(ts), 'utf-8')

        unix_out['OUT'].send(cmd_byte)

        time.sleep(1)
        print(PRINT_PREPEND + 'Done.. \n')

        inpeat = True
        while inpeat:
            input_r = input('[R] Repeat capture preserving settings \n[N] New capture \n[E] Exit program \nChoice [R, N, E] # ')
            if input_r == 'R' or input_r == 'r':
                print('\n'+PRINT_PREPEND + 'K4A Image str: ' + cmd)
                ts = int(time.time())
                cmd_byte = bytearray(cmd+'-'+str(ts), 'utf-8')
                unix_out['OUT'].send(cmd_byte)
                time.sleep(1)
                print(PRINT_PREPEND + 'Done.. \n')
            elif input_r == 'N' or input_r == 'n':
                inpeat = False
            elif input_r == 'E' or input_r == 'e':
                repeat = False
                inpeat = False
                closeAll()
                print(PRINT_PREPEND + 'END: Generating logfile.. \n')
                return 0
    return 0

if __name__ == '__main__':
    main()
