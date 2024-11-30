#!/usr/bin/python

### * **************************************** * ###
### *   Simple threaded Unix socket server     * ###
### *   running within the payload's container * ###
### * **************************************** * ###

import socket
import os, os.path
import threading
import time
import subprocess

PRINT_PREPEND = '[KINECT LUNA] '
ERROR_PREPEND = '[ERROR] '
UNIX_SOCKETS_BASE_DIR = '/tmp/payload_sockets/'
UNIX_ADDR_IN = UNIX_SOCKETS_BASE_DIR + 'pl_sock'

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
            print(self.server_address);
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
                cmd = data.decode('utf-8').split('-')
                params = list(cmd[0])
                input_error = False;

                if(cmd[0] == "SM" or cmd[0] == "sm"):
                    if(cmd[1] == "RM" or cmd[1] == "rm"):
                        if(cmd[2] == "ALL" or cmd[2] == "all"):
                            if(os.path.exists('/storage')):
                                os.system('rm /storage/*')
                                print(PRINT_PREPEND + 'All captures deleted from /storage directory.')
                        else:
                            if(os.path.exists('/storage/'+cmd[2]+'.tar.gz')):
                                os.system('rm /storage/'+cmd[2]+'.tar.gz')
                                print(PRINT_PREPEND + 'File '+cmd[2]+'.tar.gz removed from /storage directory.')

                if(len(params) == 12):
                    fps = params[1]+params[2]
                    color = params[3]+params[4]+params[5]+params[6]
                    resolution = params[7]+params[8]+params[9]+params[10]
                    depth = params[11]

                    if fps != '05' and fps != '15' and fps != '30':
                        input_error = True
                        print(ERROR_PREPEND + 'Invalid FPS value ' + fps + '.')
                    #if color != 'MJPG':
                    #    input_error = True
                    #    print(ERROR_PREPEND + 'Invalid color format ' + color + '. Currently only supports MJPG.')
                    if resolution != '0720' and resolution != '1080' and resolution != '1440' and resolution != '1536' and resolution != '2160' and resolution != '3072':
                        input_error = True
                        print(ERROR_PREPEND + 'Invalid resolution ' + resolution + '. Supported resolution:  720, 1080, 1440, 1536, 2160, 3072')
                    if depth != '0' and depth != '1' and depth != '2' and depth != '3' and depth != '4' and depth != '5':
                        input_error = True
                        print(ERROR_PREPEND + 'Invalid Depth mode value ' + depth + '.')

                    if input_error == False:
                        if fps == '05':
                            fps='5'

                        if color == 'BGRA':
                            color = 'BGRA32'
                        elif color == 'DP16':
                            color = 'DEPTH16'

                        if resolution == '0000':
                            resolution = 'OFF'
                        elif resolution == '0720':
                            resolution = '720P'
                        else:
                            resolution+='P'

                        if depth == '0':
                            depth = 'OFF'
                        elif depth == '1':
                            depth = 'NFOV_2X2BINNED'
                        elif depth == '2':
                            depth = 'NFOV_UNBINNED'
                        elif depth == '3':
                            depth = 'WFOV_2X2BINNED'
                        elif depth == '4':
                            depth = 'WFOV_UNBINNED'
                        elif depth == '5':
                            depth = 'PASSIVE_IR'

                        print(PRINT_PREPEND + 'K4A Image Capture: ' + cmd[0] + ' t:' + cmd[len(cmd)-1])
                    else:
                        input_error = True;
                        print(ERROR_PREPEND + 'The following command ' + cmd[0] + ' is not a valid capture sequence..')

                    if(len(cmd)==11 or len(cmd)==10):
                        print(PRINT_PREPEND + 'K4A Color Settings: {} {} {} {} {} {} {} {} {}'.format(cmd[1],cmd[2],cmd[3],cmd[4],cmd[5],cmd[6],cmd[7],cmd[8],cmd[9]))
                        try:
                            exposure = cmd[1].split('E')[1]
                            brightness = cmd[2].split('B')[1]
                            contrast = cmd[3].split('C')[1]
                            saturation = cmd[4].split('S')[1]
                            sharpness = cmd[5].split('H')[1]
                            gain = cmd[6].split('G')[1]
                            white_balance = cmd[7].split('W')[1]
                            blacklight_comp = cmd[8].split('P')[1]
                            powerline_freq = cmd[9].split('L')[1]
                        except IndexError:
                            input_error = False

                        if input_error == False:
                            os.system('check-device -f {} -c {} -r {} -d {} -t {} -xp {} -br {} -cn {} -st {} -sh {} -gn {} -wb {} -bl {} -pl {}'.format(fps, color, resolution, depth, cmd[len(cmd)-1], exposure, brightness, contrast, saturation, sharpness, gain, white_balance, blacklight_comp, powerline_freq))
                            print(PRINT_PREPEND + 'K4A Color Settings: {} {} {} {} {} {} {} {} {}'.format(exposure, brightness, contrast, saturation, sharpness, gain, white_balance, blacklight_comp, powerline_freq))
                    elif(len(cmd) >= 1 or len(cmd) <= 3):
                        print(PRINT_PREPEND + 'Loading K4A Default Color Settings..')
                        if input_error == False:
                            os.system('check-device -f {} -c {} -r {} -d {} -t {}'.format(fps, color, resolution, depth, cmd[len(cmd)-1]))
                            print(PRINT_PREPEND+'check-device -f {} -c {} -r {} -d {} -t {}'.format(fps, color, resolution, depth, cmd[len(cmd)-1]))
                    else:
                        input_error = True;
                        print(ERROR_PREPEND + 'The following command ' + data.decode('utf-8') + ' is not a valid capture sequence..')
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
    unix_in  = {}

    def closeAll():
        [unix_in[key].close() for key in unix_in]

    try:
        unix_in = {
            'IN'          : UNIX_Coms(UNIX_ADDR_IN),
        }
        [unix_in[key].bind_to_socket() for key in unix_in]
    except:
        print(PRINT_PREPEND + 'Error creating outgoing UNIX socket(s)')
        unix_in = None
        
    return 0

if __name__ == '__main__':
    main()
