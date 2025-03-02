#ifdef USING_VSCODE_AS_EDITOR
    #include "Imu.h"
#endif

namespace pimu
{

/* dolor de cabeza, no sabia como usar "inicializador de lista de miembros" para variables por referencia */
Imu::Imu(MPU9250 &module):_module(module),_gyro(module),_accel(module) {}

/* sets up mpu9250 communication */
int Imu::begin() {
    this->_module.begin();
    this->_initialized = true;
    return 1;
}

/* estimates gyro offsets and saves them in a file, this process with last a duration in seconds */
int Imu::calibrateGyro(int durationSeconds) {
    if (!this->_initialized) {
        std::cout << "No se pudo calibrar el sensor, porque el modulo no fue inicializado.\n";
        return -1;
    }

    std::cout << "Iniciando calibracion del giroscopio. NO MUEVA EL SENSOR. " << durationSeconds << " segundos\n";
    if (this->_gyro.calibrate(durationSeconds) < 0) {
        std::cout << "La calibracion no se pudo completar por un error.\n";
        return -1; 
    }


    {   // sets update thread for angles
        std::thread updateThread(&Imu::_update, this);
        updateThread.detach();
    }

    std::cout << "Calibracion finalizada\n";
}

/* calibrate accel through a linear fit function, this process involves pointing the sensor at diferent positions */
int Imu::calibrateAccel(int durationSeconds) {
    if (!this->_initialized) {
        std::cout << "No se pudo calibrar el sensor, porque el modulo no fue inicializado.\n";
        return -1;
    }
    std::cout << "\nIniciando calibracion del acelerometro. NO MUEVA EL SENSOR. " << durationSeconds << " segundos\n";
    this->_accel.calibrate(durationSeconds);
    std::cout << "Fin calibracion acelerometro.\n";
}

int Imu::calibrateAccel(std::string file_to_load_calibration_from) {

}

/* returns gyro, accel and mag readings as a mpu_module struct */
mpu_module Imu::read() {
    mpu_module returnData;
    
    sensor gyroData = this->_gyro.read();
    returnData.gx = gyroData.x;
    returnData.gy = gyroData.y;
    returnData.gz = gyroData.z;

    sensor accelData = this->_accel.read();
    returnData.ax = accelData.x;
    returnData.ay = accelData.y;
    returnData.az = accelData.z;

    return returnData;
}

void Imu::print(mpu_module readData) {
    std::cout << "Gyro (x, y, z): " << readData.gx << "rad/s, " << readData.gy << "rad/s, " << readData.gz << "rad/s\n";
    std::cout << "Accel (x, y, z): " << readData.ax << "G, " << readData.ay << "G, " << readData.az << "G\n";
}

/* 
    set low pass filters coefficient values for gyro readings, value should be in range [0,1] 
    obs: default is 1.0 (one)
*/
void Imu::setGyroFilters(float filter_constant) {
    this->_gyro.setFilterConstant(filter_constant);
}

/* updates x and y axis created angles */
void Imu::_update() {
    while (true)
    {
        this->_gyro.updateAngles();
        
        this->_xAngle = this->_gyro.getXAngle() / _d2r; // radians to degrees
        this->_yAngle = this->_gyro.getYAngle() / _d2r; // radians to degrees
    }
    
}

/* returns angle x axis created angle */
float Imu::getXAngle() {
    return this->_xAngle;
}

/* returns angle y axis created angle */
float Imu::getYAngle() {
    return this->_yAngle;
}


} // namespace pimu