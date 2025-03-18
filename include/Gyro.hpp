#ifdef VSCODE_INTELLISENSE_SUPPORT
#include "LowPass.hpp"
#include "MPU9250.hpp"
#include "operations.hpp"
#include "type.hpp"
#include "delay.hpp"
#endif

#include <unistd.h>
#include <iostream>
#include <string>
#include <chrono>

namespace pimu
{

class Gyro
{
public:
    /* pass mpu9250 module as parameter */
    Gyro(MPU9250 &module) : module_(module), x_axis_filter_(), y_axis_filter_(), z_axis_filter_() {
        this->setFilterConstant(1.0); // low pass filters wont have effect by default
    }
    
    /* set low pass filters coefficient values for Gyro::read(), value should be in range [0,1]  */
    void setFilterConstant(float constant) {
        x_axis_filter_.setAlpha(constant);
        y_axis_filter_.setAlpha(constant);
        z_axis_filter_.setAlpha(constant);
    }

    /* estimates the gyro biases by averaging, run this process for a duration in seconds */
    int calibrate(int durationSeconds) {
        float gxbD = 0.0f;
        float gzbD = 0.0f;
        float gybD = 0.0f;

        calibration_num_samples_ = 0;

        this->setXAxisBias(0.0f);
        this->setYAxisBias(0.0f);
        this->setZAxisBias(0.0f);

        // take samples and find bias
        auto start = std::chrono::high_resolution_clock::now();
        auto end = start + std::chrono::seconds(durationSeconds);
        while (std::chrono::high_resolution_clock::now() < end)
        {
            calibration_num_samples_++;
            module_.readSensor();
            gxbD += module_.getGyroX_rads();
            gybD += module_.getGyroY_rads();
            gzbD += module_.getGyroZ_rads();
            delay(20);
        }

        // set offsets
        this->setXAxisBias(gxbD / calibration_num_samples_);
        this->setYAxisBias(gybD / calibration_num_samples_);
        this->setZAxisBias(gzbD / calibration_num_samples_);

        return 1;
    }

    /* returns the gyro bias in the X direction, rad/s */
    float getXAxisBias() { return x_axis_bias_; }

    /* returns the gyro bias in the Y direction, rad/s */
    float getYAxisBias() {return y_axis_bias_; }

    /* returns the gyro bias in the Z direction, rad/s */
    float getZAxisBias() { return z_axis_bias_; }

    /* sets the gyro bias in the X direction to bias, [rad/s] */
    void setXAxisBias(float bias) { x_axis_bias_ = bias; }

    /* sets the gyro bias in the Y direction to bias, [rad/s] */
    void setYAxisBias(float bias) { y_axis_bias_ = bias; }

    /* sets the gyro bias in the Z direction to bias, [rad/s] */
    void setZAxisBias(float bias) { z_axis_bias_ = bias; }

    /* returns struct with the gyroscope readings in rad/s, with 2 decimals precision as 0.00 */
    Sensor read() {
        if (!x_axis_filter_.isAlphaDefined() || !y_axis_filter_.isAlphaDefined() || !z_axis_filter_.isAlphaDefined()) {
            std::cerr << "Falta definir la constante del filtro.\n";
        }

        Sensor return_data;

        // read Sensor data
        module_.readSensor();

        // return data with low pass filter
        float x_output = x_axis_filter_.filter(module_.getGyroX_rads());
        float y_output = y_axis_filter_.filter(module_.getGyroY_rads());
        float z_output = z_axis_filter_.filter(module_.getGyroZ_rads());

        // return data with offsets
        return_data.x = round(x_output - x_axis_bias_, 2);
        return_data.y = round(y_output - y_axis_bias_, 2);
        return_data.z = round(z_output - z_axis_bias_, 2);

        return return_data;
    }

    /* prints the gyro readings from Gyro::read() in a formatted output */
    void print(Sensor read_data) {   
        std::cout << "Accel (x,y,z): " << read_data.x << "kG_, " << read_data.y << "kG_, " << read_data.z << "kG_\n";
    }


    /* updates angles created from movement in the X and Y axis */
    void updateAngles() {
        // start gyro clock for angle measurement
        if (!gyro_timer_started_) clock_gettime(CLOCK_REALTIME, &gyro_prev_time_);

        gyro_timer_started_ = true;

        // Obtener tiempo transcurrido en segundos (dt)
        clock_gettime(CLOCK_REALTIME, &gyro_current_time);
        float dt = (gyro_current_time.tv_sec - gyro_prev_time_.tv_sec) +
                    (gyro_current_time.tv_nsec - gyro_prev_time_.tv_nsec) / 1e9;

        // Actualizar ángulos usando la integración
        Sensor SensorData = read();
        x_axis_angle_ += SensorData.x * dt;
        y_axis_angle_ += SensorData.y * dt;

        // Guardar el tiempo actual como referencia para la siguiente iteración
        gyro_prev_time_ = gyro_current_time;
    }

    /* returns angle x axis created angle */
    float getXAxisAngle() { return x_axis_angle_; }

    /* returns angle y axis created angle */
    float getYAxisAngle() { return y_axis_angle_; }

private:
    MPU9250 &module_;

    LowPass<float> x_axis_filter_;
    LowPass<float> y_axis_filter_;
    LowPass<float> z_axis_filter_;

    float x_axis_bias_ = 0.0f;
    float y_axis_bias_ = 0.0f;
    float z_axis_bias_ = 0.0f;

    float x_axis_angle_ = 0.0f;
    float y_axis_angle_ = 0.0f;
    int calibration_num_samples_ = 0; // calibration samples counter

    bool gyro_timer_started_ = false;
    struct timespec gyro_current_time;
    struct timespec gyro_prev_time_;
};

} // namespace pimu
