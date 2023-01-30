#include <k4a/k4a.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <chrono>
using namespace std;

long WriteToFile(const char *fileName, void *buffer, size_t bufferSize) {
  ofstream hFile;
  hFile.open(fileName, ios::out | ios::trunc | ios::binary);
  if (hFile.is_open()){
    hFile.write((char *)buffer, static_cast<streamsize>(bufferSize));
    hFile.close();
  }
  return 0;
}

int main(int argc,char* argv[]){
  string fps, color, resolution, depth, unix_time;
  string exposure, brightness, contrast, saturation;
  string sharpness, gain, white_balance, blacklight_comp, powerline_freq;

  if(argc >= 2){
    fps = argv[2];
    color = argv[4];
    resolution = argv[6];
    depth = argv[8];
    unix_time = argv[10];
    printf("[AZURE KINECT] K4A Image config: \nfps: %s, color: %s, resolution: %s, depth: %s\n", fps.c_str(), color.c_str(), resolution.c_str(), depth.c_str());
  }

  if (argc > 12){
    exposure = argv[12];
    brightness = argv[14];
    contrast = argv[16];
    saturation = argv[18];
    sharpness = argv[20];
    gain = argv[22];
    white_balance = argv[24];
    blacklight_comp = argv[26];
    powerline_freq = argv[28];
    printf("[AZURE KINECT] K4A Image config: \nfps: %s, color: %s, resolution: %s, depth: %s\n[AZURE KINECT] color settings: exposure: %s, brightness: %s, contrast: %s, saturation: %s, sharpness: %s, gain: %s, white_balance: %s, blacklight_comp: %s, powerline_freq: %s\n", fps.c_str(), color.c_str(), resolution.c_str(), depth.c_str(), exposure.c_str(), brightness.c_str(), contrast.c_str(), saturation.c_str(), sharpness.c_str(), gain.c_str(), white_balance.c_str(), blacklight_comp.c_str(), powerline_freq.c_str());
  }

  uint32_t count = k4a_device_get_installed_count();
  k4a_capture_t capture = NULL;
  int returnCode = 1;

  const int32_t TIMEOUT_IN_MS = 1000;

  if (count == 0) {
    printf("\n[AZURE KINECT] No k4a devices attached!\n");
    return 1;
  }

  // Open the first plugged in Kinect device
  k4a_device_t device = NULL;
  if (K4A_FAILED(k4a_device_open(K4A_DEVICE_DEFAULT, &device))) {
    printf("[AZURE KINECT] Failed to open k4a device!\n");
    return 1;
  }

  size_t serial_size = 0;
  k4a_device_get_serialnum(device, NULL, &serial_size);

  // Allocate memory for the serial, then acquire it
  char *serial = (char*)(malloc(serial_size));
  k4a_device_get_serialnum(device, serial, &serial_size);
  printf("[AZURE KINECT] Opened device: %s\n", serial);
  free(serial);

  // Set cameras default settings
  k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
  config.camera_fps       = K4A_FRAMES_PER_SECOND_15;
  config.color_format     = K4A_IMAGE_FORMAT_COLOR_BGRA32;
  config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
  config.depth_mode = K4A_DEPTH_MODE_NFOV_2X2BINNED;

  // Set cameras settings based on capture sequence
  if(fps=="5")
    config.camera_fps       = K4A_FRAMES_PER_SECOND_5;
  else if(fps=="15")
    config.camera_fps       = K4A_FRAMES_PER_SECOND_15;
  else if(fps=="30")
    config.camera_fps       = K4A_FRAMES_PER_SECOND_15;

  if(color=="MJPG")
    config.color_format     = K4A_IMAGE_FORMAT_COLOR_MJPG;
  else if(color=="NV12")
    config.color_format     = K4A_IMAGE_FORMAT_COLOR_NV12;
  else if(color=="YUY2")
    config.color_format     = K4A_IMAGE_FORMAT_COLOR_YUY2;
  else if(color=="BGRA32")
    config.color_format     = K4A_IMAGE_FORMAT_COLOR_BGRA32;
  else if(color=="DEPTH16")
    config.color_format     = K4A_IMAGE_FORMAT_DEPTH16;
  else if(color=="IR16")
    config.color_format     = K4A_IMAGE_FORMAT_IR16;

  if(resolution=="OFF")
    config.color_resolution     = K4A_COLOR_RESOLUTION_OFF;
  else if(resolution=="720P")
    config.color_resolution     = K4A_COLOR_RESOLUTION_720P;
  else if(resolution=="1080P")
    config.color_resolution     = K4A_COLOR_RESOLUTION_1080P;
  else if(resolution=="1440P")
    config.color_resolution     = K4A_COLOR_RESOLUTION_1440P;
  else if(resolution=="1536P")
    config.color_resolution     = K4A_COLOR_RESOLUTION_1536P;
  else if(resolution=="2160P")
    config.color_resolution     = K4A_COLOR_RESOLUTION_2160P;
  else if(resolution=="3072P")
    config.color_resolution     = K4A_COLOR_RESOLUTION_3072P;

  if(depth=="OFF")
    config.depth_mode     = K4A_DEPTH_MODE_OFF;
  else if(depth=="NFOV_2X2BINNED")
    config.depth_mode     = K4A_DEPTH_MODE_NFOV_2X2BINNED;
  else if(depth=="NFOV_UNBINNED")
    config.depth_mode     = K4A_DEPTH_MODE_NFOV_UNBINNED;
  else if(depth=="WFOV_2X2BINNED")
    config.depth_mode     = K4A_DEPTH_MODE_WFOV_2X2BINNED;
  else if(depth=="WFOV_UNBINNED")
    config.depth_mode     = K4A_DEPTH_MODE_WFOV_UNBINNED;
  else if(depth=="PASSIVE_IR")
    config.depth_mode     = K4A_DEPTH_MODE_PASSIVE_IR;

  if(argc>12){
  //Default is A
  if(exposure=="A")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_AUTO, 0);
  else if (exposure=="M1" || exposure=="M500")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 500);
  else if (exposure=="M2" || exposure=="M1250")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 1250);
  else if (exposure=="M3" || exposure=="M2500")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 2500);
  else if (exposure=="M4" || exposure=="M10000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 10000);
  else if (exposure=="M5" || exposure=="M20000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 20000);
  else if (exposure=="M6" || exposure=="M30000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 30000);
  else if (exposure=="M7" || exposure=="M40000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 40000);
  else if (exposure=="M8" || exposure=="M50000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 50000);
  else if (exposure=="M9" || exposure=="M60000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 60000);
  else if (exposure=="M10" || exposure=="M80000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 80000);
  else if (exposure=="M11" || exposure=="M100000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 100000);
  else if (exposure=="M12" || exposure=="M120000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 120000);
  else if (exposure=="M13" || exposure=="M130000")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 130000);

  //Default value is 128
  int brightness_integer = atoi(brightness.c_str());
  if(brightness_integer>=0 || brightness_integer <=255)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_BRIGHTNESS, K4A_COLOR_CONTROL_MODE_MANUAL, brightness_integer );

  //Defaul value is 5
  int contrast_integer = atoi(contrast.c_str());
  if(contrast_integer>=0 || contrast_integer <=10)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_CONTRAST, K4A_COLOR_CONTROL_MODE_MANUAL, contrast_integer );

  //Default value is 32
  int saturation_integer = atoi(saturation.c_str());
  if(saturation_integer>=0 || saturation_integer <=63)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_SATURATION, K4A_COLOR_CONTROL_MODE_MANUAL, saturation_integer );

  //Default value is 2
  int sharpness_integer = atoi(sharpness.c_str());
  if(sharpness_integer>=0 || sharpness_integer <=4)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_SHARPNESS, K4A_COLOR_CONTROL_MODE_MANUAL, sharpness_integer );

  //Default value is 0
  int gain_integer = atoi(gain.c_str());
  if(gain_integer >= 0 || gain_integer <=255)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_GAIN, K4A_COLOR_CONTROL_MODE_MANUAL, gain_integer );

  //Default is A
  if(white_balance == "A")
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_WHITEBALANCE, K4A_COLOR_CONTROL_MODE_AUTO, 0 );
  else{
      int white_balance_integer = atoi(white_balance.c_str());
      white_balance_integer = white_balance_integer - (white_balance_integer%10);
      k4a_device_set_color_control(device, K4A_COLOR_CONTROL_WHITEBALANCE, K4A_COLOR_CONTROL_MODE_MANUAL, white_balance_integer);
      printf("DEBUG: %i\n", white_balance_integer);
  }

  int blacklight_comp_integer = atoi(blacklight_comp.c_str());
  if(blacklight_comp_integer == 0 || blacklight_comp_integer ==1)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, K4A_COLOR_CONTROL_MODE_MANUAL, blacklight_comp_integer );

  int powerline_freq_integer = atoi(powerline_freq.c_str());
  if(powerline_freq_integer == 0 || powerline_freq_integer ==1)
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, K4A_COLOR_CONTROL_MODE_MANUAL, powerline_freq_integer );
  }
  // Start the camera with the given configuration
  if (K4A_FAILED(k4a_device_start_cameras(device, &config)))
  {
    printf("[AZURE KINECT] Failed to start cameras!\n");
    k4a_device_close(device);
    return 1;
  }
  int captureFrameCount = 30;
  const auto ltt = chrono::system_clock::now();
  int64_t timestamp = chrono::duration_cast<chrono::seconds>(ltt.time_since_epoch()).count() ;
  const char* path = "/storage/";
  string color_blk;
  string depth_blk;
  string ir_blk;

  while (captureFrameCount-- > 0) {
    k4a_image_t image;

    // Get a depth frame
    switch (k4a_device_get_capture(device, &capture, TIMEOUT_IN_MS))
    {
      case K4A_WAIT_RESULT_SUCCEEDED:
      break;
      case K4A_WAIT_RESULT_TIMEOUT:
      printf("[AZURE KINECT] Timed out waiting for a capture\n");
      continue;
      break;
      case K4A_WAIT_RESULT_FAILED:
      printf("[AZURE KINECT] Failed to read a capture\n");
      goto Exit;
    }

    printf("Capture");

    // Probe for a color image
    image = k4a_capture_get_color_image(capture);
    if (image)
    {
      string filename = path + to_string(timestamp) + "C" + fps + color + resolution + ".jpeg";
      color_blk = to_string(timestamp) + "C" + fps + color + resolution + ".jpeg";
      printf(" | Color res:%4dx%4d stride:%5d ",
      k4a_image_get_height_pixels(image),
      k4a_image_get_width_pixels(image),
      k4a_image_get_stride_bytes(image));
      WriteToFile(filename.c_str(), k4a_image_get_buffer( image ), k4a_image_get_size(image));
      k4a_image_release(image);
    }
    else
    {
      printf(" | Color None                       ");
    }

    // probe for a IR16 image
    image = k4a_capture_get_ir_image(capture);
    if (image != NULL)
    {
      string filename = path + to_string(timestamp) + "IR" + fps + to_string(k4a_image_get_width_pixels(image)) + depth;
      depth_blk = to_string(timestamp) + "IR" + fps + to_string(k4a_image_get_width_pixels(image)) + depth;
      printf(" | Ir16 res:%4dx%4d stride:%5d ",
      k4a_image_get_width_pixels(image),
      k4a_image_get_height_pixels(image),
      k4a_image_get_stride_bytes(image));
      WriteToFile(filename.c_str(), k4a_image_get_buffer( image ), k4a_image_get_size(image));

      k4a_image_release(image);
    }
    else
    {
      printf(" | Ir16 None                       ");
    }

    // Probe for a depth16 image
    image = k4a_capture_get_depth_image(capture);
    if (image != NULL)
    {
      string filename = path + to_string(timestamp) + "D" + fps + to_string(k4a_image_get_width_pixels(image)) + depth;
      ir_blk = to_string(timestamp) + "D" + fps + to_string(k4a_image_get_width_pixels(image)) + depth;;
      printf(" | Depth16 res:%4dx%4d stride:%5d\n",
      k4a_image_get_width_pixels(image),
      k4a_image_get_height_pixels(image),
      k4a_image_get_stride_bytes(image));
      WriteToFile(filename.c_str(), k4a_image_get_buffer( image ), k4a_image_get_size(image));

      k4a_image_release(image);
    }
    else
    {
      printf(" | Depth16 None\n");
    }

    // release capture
    k4a_capture_release(capture);
    fflush(stdout);

    if(captureFrameCount==0){
      // Shut down the camera when finished with application logic
      k4a_device_stop_cameras(device);
      k4a_device_close(device);
      //compress captured framgents
      string compress_cmd = "calibrate;cd /storage; tar -czf " + to_string(timestamp) + ".tar.gz " + color_blk + " " + depth_blk + " " + ir_blk+" calibration.json; rm calibration.json "+ color_blk + " " + depth_blk + " " + ir_blk;
      system(compress_cmd.c_str());
    }

  }

  return 0;

  returnCode = 0;
  Exit:
  if (device != NULL)
  {
    k4a_device_close(device);
  }

  return returnCode;
}
