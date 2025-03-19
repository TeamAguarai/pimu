#ifdef VSCODE_INTELLISENSE_SUPPORT
#include "MPU9250.hpp"
#include "Gyro.hpp"
#include "Accel.hpp"
#endif


#include <thread>
#include <iostream>

namespace pimu {

class Imu {
public:
    Imu(MPU9250 &module);

    int begin();
    int calibrateGyro(int duration_seconds);
    int calibrateAccel(int duration_seconds);
    MultiSensor read();
    void print(MultiSensor read_data);
    void setGyroFilters(float filter_constant);
    void startUpdateThread();
    float getXAxisAngle();
    float getYAxisAngle();

private:
    MPU9250 &module_;
    Gyro gyro_;
    Accel accel_;

    bool initialized_ = false;
    
    float x_axis_angle_ = 0.0f;
    float y_axis_angle_ = 0.0f;

    const float d2r_ = 3.14159265359f / 180.0f; 

    void startUpdateLoop();
};

/* Imu constructor */
Imu::Imu(MPU9250 &module) : module_(module), gyro_(module), accel_(module) {}

/* sets up mpu9250 communication */
int Imu::begin() {
    module_.begin();
    initialized_ = true;
    return 1;
}

/* estimates gyro offsets and applies them */
int Imu::calibrateGyro(int duration_seconds) {
    if (!initialized_) {
        std::cout << "No se pudo calibrar el Sensor, porque el modulo no fue inicializado.\n";
        return -1;
    }

    std::cout << "Iniciando calibracion del giroscopio. NO MUEVA EL Sensor. " << duration_seconds << " segundos\n";
    if (gyro_.calibrate(duration_seconds) < 0) {
        std::cout << "La calibracion no se pudo completar por un error.\n";
        return -1; 
    }

    std::cout << "Calibracion finalizada\n";
    return 1;
}

/* calibrate accel through a linear fit function, this process involves pointing the Sensor at different positions */
int Imu::calibrateAccel(int duration_seconds) {
    if (!initialized_) {
        std::cout << "No se pudo calibrar el Sensor, porque el modulo no fue inicializado.\n";
        return -1;
    }
    std::cout << "\nIniciando calibracion del acelerometro. NO MUEVA EL Sensor. " << duration_seconds << " segundos\n";
    accel_.calibrate(duration_seconds);
    std::cout << "Fin calibracion acelerometro.\n";
    return 1;
}

/* returns gyro, accel and mag readings as a MultiSensor struct */
MultiSensor Imu::read() {
    MultiSensor return_data;
    
    Sensor gyro = gyro_.read();
    return_data.gx = gyro.x;
    return_data.gy = gyro.y;
    return_data.gz = gyro.z;

    Sensor accel = accel_.read();
    return_data.ax = accel.x;
    return_data.ay = accel.y;
    return_data.az = accel.z;

    return return_data;
}

/* prints the gyro and accelerometer readings from Imu::read() in a formatted output */
void Imu::print(MultiSensor read_data) {
    std::cout << "Gyro (x, y, z): " << read_data.gx << "rad/s, " 
              << read_data.gy << "rad/s, " 
              << read_data.gz << "rad/s\n";
    std::cout << "Accel (x, y, z): " << read_data.ax << "G, " 
              << read_data.ay << "G, " 
              << read_data.az << "G\n";
}

/* 
    set low pass filters coefficient values for gyro readings, value should be in range [0,1] 
    obs: default is 1.0 (one)
*/
void Imu::setGyroFilters(float filter_constant) {
    gyro_.setFilterConstant(filter_constant);
}

/* starts thread with std::thread that updates angles measurements */
void Imu::startUpdateThread() {
    {   // sets update thread for angles
        std::thread updateThread(&Imu::startUpdateLoop, this);
        updateThread.detach();
    }
}

/* returns angle x axis created angle */
float Imu::getXAxisAngle() { return x_axis_angle_; }

/* returns angle y axis created angle */
float Imu::getYAxisAngle() { return y_axis_angle_; }

/* updates X and Y axis angles */
void Imu::startUpdateLoop() {
    while (true) {
        gyro_.updateAngles();
        
        x_axis_angle_ = gyro_.getXAxisAngle() / d2r_; // radians to degrees
        y_axis_angle_ = gyro_.getYAxisAngle() / d2r_; // radians to degrees

        delay(2);
    }
}

} // namespace pimu
