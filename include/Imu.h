#ifdef USING_VSCODE_AS_EDITOR
    #include "MPU9250.h"
    #include "Gyro.h"
    #include "Accel.h"
#endif

#include <thread>

namespace pimu
{

class Imu
{
public:
    Imu(MPU9250 &module);
    
    int begin();

    int calibrateGyro(int durationSeconds);
    int calibrateAccel(int durationSeconds);
    int calibrateAccel(std::string file_to_load_calibration_from);
    
    mpu_module read();
    void print(mpu_module readData);

    void setGyroFilters(float filter_constant);
    
    float getXAngle();
    float getYAngle();
    
    MPU9250 &_module;
    Accel _accel;
    Gyro _gyro;
private:

    bool _initialized = false;
    
    float _xAngle = 0.0;
    float _yAngle = 0.0;

    const float _d2r = 3.14159265359f/180.0f; 

    void _update();
    
};

} // namespace pimu
