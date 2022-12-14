#pragma comment(lib, "k4a.lib")
#include <k4a/k4a.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <chrono>

long WriteToFile(const char *fileName, void *buffer, size_t bufferSize) {
  std::ofstream hFile;
  hFile.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
  if (hFile.is_open()){
    hFile.write((char *)buffer, static_cast<std::streamsize>(bufferSize));
    hFile.close();
  }
  return 0;
}

int main(int argc,char* argv[]){
  std::string fps, color, resolution, depth, unix_time;
  std::string exposure, brightness, contrast, saturation;
  std::string sharpness, gain, white_balance, blacklight_comp, powerline_freq;

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

  // Set cameras settings based on cmd args
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

  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_AUTO, 130000 );
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_BRIGHTNESS, K4A_COLOR_CONTROL_MODE_MANUAL, 255 ); 0 - 255
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_CONTRAST, K4A_COLOR_CONTROL_MODE_MANUAL, 255 );
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_SATURATION, K4A_COLOR_CONTROL_MODE_MANUAL, 0 );
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_SHARPNESS, K4A_COLOR_CONTROL_MODE_MANUAL, 255 );
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_WHITEBALANCE, K4A_COLOR_CONTROL_MODE_AUTO, 2500 );
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, K4A_COLOR_CONTROL_MODE_MANUAL, 0 );
  //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_POWERLINE_FREQUENCY, K4A_COLOR_CONTROL_MODE_MANUAL, 1 );

  // Start the camera with the given configuration
  if (K4A_FAILED(k4a_device_start_cameras(device, &config)))
  {
    printf("[AZURE KINECT] Failed to start cameras!\n");
    k4a_device_close(device);
    return 1;
  }
  int captureFrameCount = 3;
  const auto ltt = std::chrono::system_clock::now();
  int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(ltt.time_since_epoch()).count() ;
  const char* path = "/storage/";
  std::string color_blk;
  std::string depth_blk;
  std::string ir_blk;

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
      std::string filename = path + std::to_string(timestamp) + "C" + fps + color + resolution + ".jpeg";
      color_blk = std::to_string(timestamp) + "C" + fps + color + resolution + ".jpeg";
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
      std::string filename = path + std::to_string(timestamp) + "IR" + fps + std::to_string(k4a_image_get_width_pixels(image)) + depth;
      depth_blk = std::to_string(timestamp) + "IR" + fps + std::to_string(k4a_image_get_width_pixels(image)) + depth;
      printf(" | Ir16 res:%4dx%4d stride:%5d ",
      k4a_image_get_height_pixels(image),
      k4a_image_get_width_pixels(image),
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
      std::string filename = path + std::to_string(timestamp) + "D" + fps + std::to_string(k4a_image_get_width_pixels(image)) + depth;
      ir_blk = std::to_string(timestamp) + "D" + fps + std::to_string(k4a_image_get_width_pixels(image)) + depth;;
      printf(" | Depth16 res:%4dx%4d stride:%5d\n",
      k4a_image_get_height_pixels(image),
      k4a_image_get_width_pixels(image),
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
      //compress captured framgents
      std::string compress_cmd = "cd /storage; tar -czf " + std::to_string(timestamp) + ".tar.gz " + color_blk + " " + depth_blk + " " + ir_blk+"; rm "+ color_blk + " " + depth_blk + " " + ir_blk;
      system(compress_cmd.c_str());
    }

  }
  // Shut down the camera when finished with application logic
  k4a_device_stop_cameras(device);
  k4a_device_close(device);

  return 0;

  returnCode = 0;
  Exit:
  if (device != NULL)
  {
    k4a_device_close(device);
  }

  return returnCode;
}
