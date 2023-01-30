#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <k4a/k4a.h>
using namespace std;

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

static void calibration_blob(uint8_t deviceIndex = 0, string filename = "calibration.json")
{
    k4a_device_t device = NULL;

    if (K4A_RESULT_SUCCEEDED != k4a_device_open(deviceIndex, &device))
    {
        cout << deviceIndex << ": Failed to open device" << endl;
        exit(-1);
    }

    size_t calibration_size = 0;
    k4a_buffer_result_t buffer_result = k4a_device_get_raw_calibration(device, NULL, &calibration_size);
    if (buffer_result == K4A_BUFFER_RESULT_TOO_SMALL)
    {
        vector<uint8_t> calibration_buffer = vector<uint8_t>(calibration_size);
        buffer_result = k4a_device_get_raw_calibration(device, calibration_buffer.data(), &calibration_size);
        if (buffer_result == K4A_BUFFER_RESULT_SUCCEEDED)
        {
            ofstream file(filename, ofstream::binary);
            file.write(reinterpret_cast<const char *>(&calibration_buffer[0]), (long)calibration_size);
            file.close();
            cout << "Calibration blob for device " << (int)deviceIndex << " (serial no. " << get_serial(device)
                 << ") is saved to " << filename << endl;
        }
        else
        {
            cout << "Failed to get calibration blob" << endl;
            exit(-1);
        }
    }
    else
    {
        cout << "Failed to get calibration blob size" << endl;
        exit(-1);
    }
}


int main(int argc, char **argv)
{

        calibration_blob(0, "/storage/calibration.json");


    return 0;
}
