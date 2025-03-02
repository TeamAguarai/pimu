#ifdef USING_VSCODE_AS_EDITOR
    #include "LowPass.h"
    #include "MPU9250.h"
    #include "type.h"
    #include "LinearRegression.h"
#endif

#include <chrono>

namespace pimu
{
class Accel
{
public:
    Accel(MPU9250 &module);

    int calibrate(int durationSeconds);

    sensor read();

    /* accelerometer calibration works trough a linear fit function that returns the appropiate value from an uncalibrated reading */

    LinearRegression getXOffset();
    LinearRegression getYOffset();
    LinearRegression getZOffset();

    MPU9250 &_module;
private:

    bool _calibrated = false;
    
    float _xbias;
    float _ybias;
    float _zbias;
    
    LinearRegression _xOffset;
    LinearRegression _yOffset;
    LinearRegression _zOffset;



    const float G = 9.807f;
};
}
