#ifdef VSCODE_INTELLISENSE_SUPPORT
#include "LowPass.hpp"
#include "MPU9250.hpp"
#include "type.hpp"
#include "LinearRegression.hpp"
#endif

#include <chrono>
#include <iostream>
#include <cmath>

namespace pimu {

class Accel {
public:
    explicit Accel(MPU9250 &module);

    int calibrate(int duration_seconds);
    Sensor read();
    void print(Sensor read_data);
    void updateAngles();
    float getXAxisAngle();
    float getYAxisAngle();
    float getXBias();
    float getYBias();
    float getZBias();

private:
    MPU9250 &module_;

    float x_axis_angle_ = 0.0f;
    float y_axis_angle_ = 0.0f;

    int calibration_num_samples_ = 0;
    bool calibrated_ = false;

    float x_bias_ = 0.0f;
    float y_bias_ = 0.0f;
    float z_bias_ = 0.0f;

    const float kG_ = 9.807f;
};

Accel::Accel(MPU9250 &module) : module_(module) {}

/* calibrate accel offsets */
int Accel::calibrate(int duration_seconds) {
    float ax_sum = 0.0f, ay_sum = 0.0f, az_sum = 0.0f;
    calibration_num_samples_ = 0;
    x_bias_ = y_bias_ = z_bias_ = 0;

    // take samples and find bias
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::seconds(duration_seconds);
    while (std::chrono::high_resolution_clock::now() < end) {
        calibration_num_samples_++;

        module_.readSensor();
        ax_sum += module_.getAccelX_mss() / kG_;
        ay_sum += module_.getAccelY_mss() / kG_;
        az_sum += module_.getAccelZ_mss() / kG_;

        delay(20);
    }

    // set offsets
    x_bias_ = ax_sum / calibration_num_samples_;
    y_bias_ = ay_sum / calibration_num_samples_;
    z_bias_ = az_sum / calibration_num_samples_;

    return 1;
}

/* returns accelerometer readings [G] */
Sensor Accel::read() {
    Sensor return_data;
    module_.readSensor();

    // convert m/s/s to kG_ minus offset
    return_data.x = round((module_.getAccelX_mss() / kG_) - x_bias_, 2);
    return_data.y = round((module_.getAccelY_mss() / kG_) - y_bias_, 2);
    return_data.z = round((module_.getAccelZ_mss() / kG_) - z_bias_, 2);

    return return_data;
}

/* prints in a formatted way the return from Accel::read() */
void Accel::print(Sensor read_data) {
    std::cout << "Accel (x,y,z): " << read_data.x << " G, " 
              << read_data.y << " G, " 
              << read_data.z << " G\n";
}

/* updates angles made from the X and Y axis [sexagesimal degree] */
void Accel::updateAngles() {
    Sensor reading = read();
    x_axis_angle_ = atan(reading.x / sqrt(reading.y * reading.y + reading.z * reading.z));
    y_axis_angle_ = atan(reading.y / sqrt(reading.x * reading.x + reading.z * reading.z));
}

/* returns X axis angle, should be called after Accel::updateAngles() [sexagesimal degree] */
float Accel::getXAxisAngle() { return x_axis_angle_; }

/* returns Y axis angle, should be called after Accel::updateAngles() [sexagesimal degree] */
float Accel::getYAxisAngle() { return y_axis_angle_; }

/* returns X axis offset, 0 by default, only changes after Accel::calibrate() */
float Accel::getXBias() { return x_bias_; }

/* returns Y axis offset, 0 by default, only changes after Accel::calibrate() */
float Accel::getYBias() { return y_bias_; }

/* returns Z axis offset, 0 by default, only changes after Accel::calibrate() */
float Accel::getZBias() { return z_bias_; }

} // namespace pimu
