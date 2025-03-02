#ifdef USING_VSCODE_AS_EDITOR
    #include "Accel.h"
#endif

namespace pimu
{

Accel::Accel(MPU9250 &module):_module(module), _xOffset(), _yOffset(), _zOffset() {
    // default calibration values (no bias, 0 scale factor)
    this->_xOffset.setCoefficients(1.0f, 0);
    this->_yOffset.setCoefficients(1.0f, 0);
    this->_zOffset.setCoefficients(1.0f, 0);
}

/* calibrate accel offsets through a linear fit function, this process involves pointing the sensor at diferent positions
referencia: https://www.youtube.com/watch?v=hoNeIaXc0vs */
int Accel::calibrate(int durationSeconds) {

    std::string axes[3] = {"X", "Y", "Z"};
    int durationPerAxis = durationSeconds / 9; 
    float sample, readingShouldBe;

    // array de punteros para poder editar por referencia
    LinearRegression* offsets[3] = {&this->_xOffset, &this->_yOffset, &this->_zOffset};

    std::string position, axis;
    sensor reading;
    int c = 1;

    for(int i = 0; i < 3; i++) {
        axis = axes[i];
        offsets[i]->zero();

        for (int j = 0; j < 3; j++) {

            if (j == 0) {
                position = "hacia el cielo";
                readingShouldBe = 1.0f; // 1G
            }
            else if (j == 1) {
                position = "hacia el suelo";
                readingShouldBe = -1.0f; // -1G
            }
            else {
                readingShouldBe = 0.0f; // 0G
                position = "perpendicular a la gravedad";
            }

            std::cout <<  "[" << c << "/9] Apunte el eje " << axis << " apuntando " << position  << " y mantenga el sensor quieto. ENTER para iniciar.";
            c++;

            // wait for ENTER key
            std::string s;
            std::getline(std::cin, s);
            std::cout << "cargando valores...\n";

            auto start = std::chrono::high_resolution_clock::now();
            auto end = start + std::chrono::seconds(durationPerAxis);
            while (std::chrono::high_resolution_clock::now() < end) {
                reading = this->read();
                
                if (axis == "X") sample = reading.x;
                if (axis == "Y") sample = reading.y;
                if (axis == "Z") sample = reading.z;

                offsets[i]->addDataPoint(readingShouldBe, sample - readingShouldBe);
            }
        }
    
        offsets[i]->computeCoefficients();
    }

    this->_calibrated = true;
}

/* returns the accelerometer measurement in the (x, y, z) direction, in G */
sensor Accel::read() {
    sensor returnData;

    this->_module.readSensor();
    float rawX = this->_module.getAccelX_mss() / G;
    float rawY = this->_module.getAccelY_mss() / G;
    float rawZ = this->_module.getAccelZ_mss() / G;

    // convert m/s/s to G
    if (this->_calibrated) {
        // return data with offset
        std::cout << "calib data\n";
        returnData.x = rawX + this->_xOffset.predict(rawX);    
        returnData.y = rawY + this->_yOffset.predict(rawY);    
        returnData.z = rawZ + this->_zOffset.predict(rawZ);   
    } else {
        // return data with no offset
        returnData.x = rawX;    
        returnData.y = rawY;    
        returnData.z = rawZ;   
    }
    
    return returnData;
}

/* returns x axis offset linear fit function */
LinearRegression Accel::getXOffset() {
    return this->_xOffset;
}

/* returns y axis offset linear fit function */
LinearRegression Accel::getYOffset() {
    return this->_yOffset;
}

/* returns z axis offset linear fit function */
LinearRegression Accel::getZOffset() {
    return this->_zOffset;
}

} // namespace pimu