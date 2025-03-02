#ifdef USING_VSCODE_AS_EDITOR
    #include "LowPass.h"
    #include "MPU9250.h"
    #include "type.h"
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
    Gyro(MPU9250 &module);

    int calibrate(int durationSeconds);
    int calibrate(int durationSeconds, std::string file_to_save_calibdata);
    int calibrate(std::string file_to_load_calibration_from);

    sensor read();

    void updateAngles();
    float getXAngle();
    float getYAngle();
    
    float getBiasX_rads();
    float getBiasY_rads();
    float getBiasZ_rads();
    void setBiasX_rads(float bias);
    void setBiasY_rads(float bias);
    void setBiasZ_rads(float bias);

    void setFilterConstant(float constant);

    MPU9250 &_module;
private:
    pimu::LowPass<float> _filters[3]; // one for each axis (x,y,z)

    float _xbias = 0.0, _ybias = 0.0, _zbias = 0.0; // bias
    float _xAngle = 0, _yAngle = 0; // angles
    int _calibrationNumSamples = 0; // calibration samples counter

    bool _gyroTimerStarted = false;
    struct timespec _gyroCurrentTime;
    struct timespec _gyroPrevTime;  

    int _calibration(int durationSeconds);

};

} // namespace pimu
