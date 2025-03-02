#ifdef USING_VSCODE_AS_EDITOR
    #include "Gyro.h"
#endif

namespace pimu
{

/* pass mpu9250 module as parameter */
Gyro::Gyro(MPU9250 &module):_module(module) {
    // low pass filters wont have effect by default 
    this->setFilterConstant(1.0); 
}

/* estimates gyro offsets */
int Gyro::calibrate(int durationSeconds) {
    this->_calibration(durationSeconds);

    return 1;
}

/* estimates gyro offsets and saves them in a file */
int Gyro::calibrate(int durationSeconds, std::string file_to_save_calibdata) {
    this->_calibration(durationSeconds);

    // writes file with calib data
    Writer writer(file_to_save_calibdata, "Calibration Data");
    writer.write_line("Gyro Samples: " + std::to_string(this->_calibrationNumSamples));
    writer.write_row({"gxbias", "gybias", "gzbias"});
    writer.write_row({
        std::to_string(this->getBiasX_rads()), 
        std::to_string(this->getBiasY_rads()), 
        std::to_string(this->getBiasZ_rads())
    });

    return 1;
}

/* load calibration data from a file created by Gyro::calibrate(int durationSeconds, std::string file_to_save_calibdata) */
int Gyro::calibrate(std::string file_to_load_calibration_from) {
    return 1;
}

/* estimates the gyro biases by averaging, run this process for a duration in seconds */
int Gyro::_calibration(int durationSeconds) {

    this->_calibrationNumSamples = 0;
    float gxbD = 0.0f;
    float gzbD = 0.0f;
    float gybD = 0.0f;
    this->setBiasX_rads(0.0f);
    this->setBiasY_rads(0.0f);
    this->setBiasZ_rads(0.0f);

    // take samples and find bias
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::seconds(durationSeconds);
    while (std::chrono::high_resolution_clock::now() < end) {
        this->_calibrationNumSamples++;
        this->_module.readSensor();
        gxbD += this->_module.getGyroX_rads();
        gybD += this->_module.getGyroY_rads();
        gzbD += this->_module.getGyroZ_rads();
        usleep(200000); // suspend 20 miliseconds
    }

    // set offsets
    this->setBiasX_rads(gxbD / this->_calibrationNumSamples);
    this->setBiasY_rads(gybD / this->_calibrationNumSamples);
    this->setBiasZ_rads(gzbD / this->_calibrationNumSamples);

    return 1;
}

/* returns the gyro bias in the X direction, rad/s */
float Gyro::getBiasX_rads() {
    return this->_xbias;
}

/* returns the gyro bias in the Y direction, rad/s */
float Gyro::getBiasY_rads() {
    return this->_ybias;
}

/* returns the gyro bias in the Z direction, rad/s */
float Gyro::getBiasZ_rads() {
    return this->_zbias;
}

/* sets the gyro bias in the X direction to bias, rad/s */
void Gyro::setBiasX_rads(float bias) {
    this->_xbias = bias;
}

/* sets the gyro bias in the Y direction to bias, rad/s */
void Gyro::setBiasY_rads(float bias) {
    this->_ybias = bias;
}

/* sets the gyro bias in the Z direction to bias, rad/s */
void Gyro::setBiasZ_rads(float bias) {
    this->_zbias = bias;
}

/* returns struct with the gyroscope readings [p_gyro.x, p_gyro.y, p_gyro.z] with 2 decimals precision  */
sensor Gyro::read() {
    if (!this->_filters[0].isAlphaDefined() || !this->_filters[1].isAlphaDefined() || !this->_filters[2].isAlphaDefined()) {
        std::cerr << "Falta definir la constante del filtro.\n";
    }
    
    sensor returnData;

    // read sensor data
    this->_module.readSensor();

    // return data with low pass filter
    float xOutput = this->_filters[0].filter(this->_module.getGyroX_rads());
    float yOutput = this->_filters[1].filter(this->_module.getGyroY_rads());
    float zOutput = this->_filters[2].filter(this->_module.getGyroZ_rads());

    // return data with offsets
    returnData.x = round(xOutput - this->_xbias, 2);
    returnData.y = round(yOutput - this->_ybias, 2);
    returnData.z = round(zOutput - this->_zbias, 2);

    return returnData;
}

/* updates angles created from movement in the X and Y axis */
void Gyro::updateAngles() {
    // start gyro clock for angle measurement
    if (!_gyroTimerStarted) clock_gettime(CLOCK_REALTIME, &_gyroPrevTime);
    _gyroTimerStarted = true;
    
    // Obtener tiempo transcurrido en segundos (dt)
    clock_gettime(CLOCK_REALTIME, &_gyroCurrentTime);
    float dt = (_gyroCurrentTime.tv_sec - _gyroPrevTime.tv_sec) + 
                (_gyroCurrentTime.tv_nsec - _gyroPrevTime.tv_nsec) / 1e9;
    
    // Actualizar ángulos usando la integración
    sensor sensorData = this->read();
    this->_xAngle += sensorData.x * dt;
    this->_yAngle += sensorData.y * dt;

    // Guardar el tiempo actual como referencia para la siguiente iteración
    _gyroPrevTime = _gyroCurrentTime;
}

/* returns angle x axis created angle */
float Gyro::getXAngle() {
    return this->_xAngle;
}

/* returns angle y axis created angle */
float Gyro::getYAngle() {
    return this->_yAngle;
}



/* set low pass filters coefficient values for Gyro::read(), value should be in range [0,1]  */
void Gyro::setFilterConstant(float constant) {
    this->_filters[0].setAlpha(constant);
    this->_filters[1].setAlpha(constant);
    this->_filters[2].setAlpha(constant);
}

}; // namespace pimu