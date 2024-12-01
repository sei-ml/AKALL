#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <k4a/k4a.h>

using namespace std;

static string get_depth_mode_string(k4a_depth_mode_t depth_mode);
static string get_color_resolution_string(k4a_color_resolution_t color_resolution);
static string get_serial(k4a_device_t device);
static void save_calibration_data(k4a_device_t device, k4a_depth_mode_t depth_mode, k4a_color_resolution_t color_resolution, const string &filename);

static string get_serial(k4a_device_t device)
{
    size_t serial_number_length = 0;

    if (K4A_BUFFER_RESULT_TOO_SMALL != k4a_device_get_serialnum(device, NULL, &serial_number_length))
    {
        cout << "Failed to get serial number length" << endl;
        k4a_device_close(device);
        exit(-1);
    }

    char *serial_number = new (std::nothrow) char[serial_number_length];
    if (serial_number == NULL)
    {
        cout << "Failed to allocate memory for serial number (" << serial_number_length << " bytes)" << endl;
        k4a_device_close(device);
        exit(-1);
    }

    if (K4A_BUFFER_RESULT_SUCCEEDED != k4a_device_get_serialnum(device, serial_number, &serial_number_length))
    {
        cout << "Failed to get serial number" << endl;
        delete[] serial_number;
        serial_number = NULL;
        k4a_device_close(device);
        exit(-1);
    }

    string s(serial_number);
    delete[] serial_number;
    serial_number = NULL;
    return s;
}

static void save_calibration_data(k4a_device_t device, k4a_depth_mode_t depth_mode, k4a_color_resolution_t color_resolution, const string &filename)
{
    k4a_calibration_t calibration;
    if (K4A_RESULT_SUCCEEDED != k4a_device_get_calibration(device, depth_mode, color_resolution, &calibration))
    {
        cout << "Failed to get camera calibration for depth mode: " << depth_mode << " and color resolution: " << color_resolution << endl;
        return;
    }

    ofstream file(filename);
    if (file.is_open())
    {
        file << "{" << endl;
        file << "  \"DepthMode\": \"" << get_depth_mode_string(depth_mode) << "\"," << endl;
        file << "  \"ColorResolution\": \"" << get_color_resolution_string(color_resolution) << "\"," << endl;
        file << "  \"DepthCameraCalibration\": {" << endl;
        file << "    \"ResolutionWidth\": " << calibration.depth_camera_calibration.resolution_width << "," << endl;
        file << "    \"ResolutionHeight\": " << calibration.depth_camera_calibration.resolution_height << "," << endl;
        file << "    \"Intrinsics\": {" << endl;
        file << "      \"FocalLengthX\": " << calibration.depth_camera_calibration.intrinsics.parameters.param.fx << "," << endl;
        file << "      \"FocalLengthY\": " << calibration.depth_camera_calibration.intrinsics.parameters.param.fy << "," << endl;
        file << "      \"PrincipalPointX\": " << calibration.depth_camera_calibration.intrinsics.parameters.param.cx << "," << endl;
        file << "      \"PrincipalPointY\": " << calibration.depth_camera_calibration.intrinsics.parameters.param.cy << "," << endl;
        file << "      \"RadialDistortion\": [" << calibration.depth_camera_calibration.intrinsics.parameters.param.k1 << ", "
             << calibration.depth_camera_calibration.intrinsics.parameters.param.k2 << ", "
             << calibration.depth_camera_calibration.intrinsics.parameters.param.k3 << "]," << endl;
        file << "      \"TangentialDistortion\": [" << calibration.depth_camera_calibration.intrinsics.parameters.param.p1 << ", "
             << calibration.depth_camera_calibration.intrinsics.parameters.param.p2 << "]" << endl;
        file << "    }," << endl;
        file << "    \"Extrinsics\": {" << endl;
        file << "      \"Rotation\": [" << calibration.depth_camera_calibration.extrinsics.rotation[0] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[1] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[2] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[3] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[4] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[5] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[6] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[7] << ", "
             << calibration.depth_camera_calibration.extrinsics.rotation[8] << "]," << endl;
        file << "      \"Translation\": [" << calibration.depth_camera_calibration.extrinsics.translation[0] << ", "
             << calibration.depth_camera_calibration.extrinsics.translation[1] << ", "
             << calibration.depth_camera_calibration.extrinsics.translation[2] << "]" << endl;
        file << "    }" << endl;
        file << "  }," << endl;
        file << "  \"ColorCameraCalibration\": {" << endl;
        file << "    \"ResolutionWidth\": " << calibration.color_camera_calibration.resolution_width << "," << endl;
        file << "    \"ResolutionHeight\": " << calibration.color_camera_calibration.resolution_height << "," << endl;
        file << "    \"Intrinsics\": {" << endl;
        file << "      \"FocalLengthX\": " << calibration.color_camera_calibration.intrinsics.parameters.param.fx << "," << endl;
        file << "      \"FocalLengthY\": " << calibration.color_camera_calibration.intrinsics.parameters.param.fy << "," << endl;
        file << "      \"PrincipalPointX\": " << calibration.color_camera_calibration.intrinsics.parameters.param.cx << "," << endl;
        file << "      \"PrincipalPointY\": " << calibration.color_camera_calibration.intrinsics.parameters.param.cy << "," << endl;
        file << "      \"RadialDistortion\": [" << calibration.color_camera_calibration.intrinsics.parameters.param.k1 << ", "
             << calibration.color_camera_calibration.intrinsics.parameters.param.k2 << ", "
             << calibration.color_camera_calibration.intrinsics.parameters.param.k3 << "]," << endl;
        file << "      \"TangentialDistortion\": [" << calibration.color_camera_calibration.intrinsics.parameters.param.p1 << ", "
             << calibration.color_camera_calibration.intrinsics.parameters.param.p2 << "]" << endl;
        file << "    }" << endl;
        file << "  }" << endl;
        file << "}" << endl;
        file.close();
        cout << "Calibration data saved to " << filename << endl;
    }
    else
    {
        cout << "Failed to open file for saving calibration data: " << filename << endl;
    }
}

static string get_depth_mode_string(k4a_depth_mode_t depth_mode)
{
    switch (depth_mode)
    {
    case K4A_DEPTH_MODE_OFF:
        return "OFF";
    case K4A_DEPTH_MODE_NFOV_2X2BINNED:
        return "NFOV_B";
    case K4A_DEPTH_MODE_NFOV_UNBINNED:
        return "NFOV_U";
    case K4A_DEPTH_MODE_WFOV_2X2BINNED:
        return "WFOV_B";
    case K4A_DEPTH_MODE_WFOV_UNBINNED:
        return "WFOV_U";
    case K4A_DEPTH_MODE_PASSIVE_IR:
        return "PIR";
    default:
        return "UNKNOWN";
    }
}

static string get_color_resolution_string(k4a_color_resolution_t color_resolution)
{
    switch (color_resolution)
    {
    case K4A_COLOR_RESOLUTION_OFF:
        return "OFF";
    case K4A_COLOR_RESOLUTION_720P:
        return "720P";
    case K4A_COLOR_RESOLUTION_1080P:
        return "1080P";
    case K4A_COLOR_RESOLUTION_1440P:
        return "1440P";
    case K4A_COLOR_RESOLUTION_1536P:
        return "1536P";
    case K4A_COLOR_RESOLUTION_2160P:
        return "2160P";
    case K4A_COLOR_RESOLUTION_3072P:
        return "3072P";
    default:
        return "UNKNOWN";
    }
}

int main(int argc, char **argv)
{
    k4a_device_t device = NULL;

    if (K4A_RESULT_SUCCEEDED != k4a_device_open(0, &device))
    {
        cout << "Failed to open Azure Kinect device" << endl;
        return -1;
    }

    k4a_depth_mode_t depth_modes[] = {
        K4A_DEPTH_MODE_OFF,
        K4A_DEPTH_MODE_NFOV_2X2BINNED,
        K4A_DEPTH_MODE_NFOV_UNBINNED,
        K4A_DEPTH_MODE_WFOV_2X2BINNED,
        K4A_DEPTH_MODE_WFOV_UNBINNED,
        K4A_DEPTH_MODE_PASSIVE_IR
    };

    k4a_color_resolution_t color_resolutions[] = {
        K4A_COLOR_RESOLUTION_OFF,
        K4A_COLOR_RESOLUTION_720P,
        K4A_COLOR_RESOLUTION_1080P,
        K4A_COLOR_RESOLUTION_1440P,
        K4A_COLOR_RESOLUTION_1536P,
        K4A_COLOR_RESOLUTION_2160P,
        K4A_COLOR_RESOLUTION_3072P
    };

    string serial_number = get_serial(device);
    for (k4a_depth_mode_t depth_mode : depth_modes)
    {
        for (k4a_color_resolution_t color_resolution : color_resolutions)
        {
            string depth_mode_str = get_depth_mode_string(depth_mode);
            string color_resolution_str = get_color_resolution_string(color_resolution);
            string filename = color_resolution_str + "_" + depth_mode_str + "_calibration.json";

            save_calibration_data(device, depth_mode, color_resolution, filename);
        }
    }

    k4a_device_close(device);
    return 0;
}