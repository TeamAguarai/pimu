#ifdef USING_VSCODE_AS_EDITOR
    #include "MPU9250.h"
    #include "Gyro.h"
#endif

#include <thread>

namespace pimu
{

class Imu
{
public:
    Imu();
    
    int begin();
    
    int calibrateAccel(int durationSeconds);
    int calibrateAccel(std::string file_to_load_calibration_from);
    
    mpu_module read();
    void print(mpu_module readData);

    void setGyroFilters(float filter_constant);
    
    float getXAngle();
    float getYAngle();
    
private:
    MPU9250 _module;
    Gyro _gyro;

    bool _initialized = false;
    
    float _xAngle = 0.0;
    float _yAngle = 0.0;

    const float _d2r = 3.14159265359f/180.0f; 

    void _update();
    int _calibrateGyro(int durationSeconds);
    
};

} // namespace pimu
