/* 
MPU9250.h: Este archivo es una fusion de los sistemas creados por ranranff (GitHub)
Creditos: https://github.com/ranranff/mpu9250/
*/

#ifdef USING_VSCODE_AS_EDITOR
    #include "Writer.h"
    #include "operations.h"
    #include "I2Cdev.h"
    #include "LowPass.h"
#endif

#include <string>
#include <cmath>

/*
MPU9250.h
Brian R Taylor
brian.taylor@bolderflight.com

Copyright (c) 2017 Bolder Flight Systems

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/



namespace pimu {

class MPU9250{
public:
    enum GyroRange
    {
        GYRO_RANGE_250DPS,
        GYRO_RANGE_500DPS,
        GYRO_RANGE_1000DPS,
        GYRO_RANGE_2000DPS
    };
    enum AccelRange
    {
        ACCEL_RANGE_2G,
        ACCEL_RANGE_4G,
        ACCEL_RANGE_8G,
        ACCEL_RANGE_16G
    };
    enum DlpfBandwidth
    {
        DLPF_BANDWIDTH_184HZ,
        DLPF_BANDWIDTH_92HZ,
        DLPF_BANDWIDTH_41HZ,
        DLPF_BANDWIDTH_20HZ,
        DLPF_BANDWIDTH_10HZ,
        DLPF_BANDWIDTH_5HZ
    };
    enum LpAccelOdr
    {
        LP_ACCEL_ODR_0_24HZ = 0,
        LP_ACCEL_ODR_0_49HZ = 1,
        LP_ACCEL_ODR_0_98HZ = 2,
        LP_ACCEL_ODR_1_95HZ = 3,
        LP_ACCEL_ODR_3_91HZ = 4,
        LP_ACCEL_ODR_7_81HZ = 5,
        LP_ACCEL_ODR_15_63HZ = 6,
        LP_ACCEL_ODR_31_25HZ = 7,
        LP_ACCEL_ODR_62_50HZ = 8,
        LP_ACCEL_ODR_125HZ = 9,
        LP_ACCEL_ODR_250HZ = 10,
        LP_ACCEL_ODR_500HZ = 11
    };

    MPU9250();
    int begin();

    int setAccelRange(AccelRange range);
    int setGyroRange(GyroRange range);
    int setDlpfBandwidth(DlpfBandwidth bandwidth);
    int setSrd(uint8_t srd);
    int enableDataReadyInterrupt();
    int disableDataReadyInterrupt();
    int enableWakeOnMotion(float womThresh_mg,LpAccelOdr odr);

    int readSensor();
    float getAccelX_mss();
    float getAccelY_mss();
    float getAccelZ_mss();
    float getGyroX_rads();
    float getGyroY_rads();
    float getGyroZ_rads();
    float getMagX_uT();
    float getMagY_uT();
    float getMagZ_uT();
    float getTemperature_C();
    bool initialized = false;
    int initCode = 0;
protected:

    // i2c
    uint8_t _address = 0x68; // I2C address

    const uint32_t _i2cRate = 400000; // 400 kHz
    size_t _numBytes = 0; // number of bytes received from I2C
    // spi
    uint8_t _csPin = 0;
    bool _useSPI   = false;
    bool _useSPIHS = false ;
    const uint8_t SPI_READ = 0x80;
    const uint32_t SPI_LS_CLOCK = 1000000;  // 1 MHz
    const uint32_t SPI_HS_CLOCK = 15000000; // 15 MHz
    // track success of interacting with sensor
    int _status = 0;
    // buffer for reading from sensor
    uint8_t _buffer[21];
    // data counts
    int16_t _axcounts = 0;
    int16_t _aycounts = 0;
    int16_t _azcounts = 0;
    int16_t _gxcounts = 0;
    int16_t _gycounts = 0;
    int16_t _gzcounts = 0;
    int16_t _hxcounts = 0;
    int16_t _hycounts = 0;
    int16_t _hzcounts = 0;
    int16_t _tcounts  = 0;
    // data buffer
    float _ax = 0.0f;
    float _ay = 0.0f;
    float _az = 0.0f;
    float _gx = 0.0f;
    float _gy = 0.0f;
    float _gz = 0.0f;
    float _hx = 0.0f;
    float _hy = 0.0f;
    float _hz = 0.0f;
    float _t  = 0.0f;
    // wake on motion
    uint8_t _womThreshold = 0;
    // scale factors
    float _accelScale = 0.0f;
    float _gyroScale  = 0.0f;
    
    float _magScaleX  = 0.0f;
    float _magScaleY  = 0.0f;
    float _magScaleZ  = 0.0f;

    const float _tempScale  = 333.87f;
    const float _tempOffset = 21.0f;
    
    // default configuration
    AccelRange _accelRange   = ACCEL_RANGE_2G;
    GyroRange _gyroRange     = GYRO_RANGE_250DPS;
    DlpfBandwidth _bandwidth = DLPF_BANDWIDTH_184HZ;
    uint8_t _srd = 0;

    // gyro bias estimation
    size_t _numSamples = 100;
    size_t _gyroNumSamples = 0;
    float _gxbD = 0.0;
    float _gybD = 0.0;
    float _gzbD = 0.0;
    float  _gxb  = 0.0f;
    float  _gyb  = 0.0f;
    float  _gzb  = 0.0f;

    // accel bias and scale factor estimation
    double _axbD  = 0.0;
    double _aybD  = 0.0;
    double _azbD  = 0.0;
    float _axmax  = 0.0f;
    float _aymax  = 0.0f;
    float _azmax  = 0.0f;
    float _axmin  = 0.0f;
    float _aymin  = 0.0f;
    float _azmin  = 0.0f;
    float _axb    = 0.0f;
    float _ayb    = 0.0f;
    float _azb    = 0.0f;
    float _axs = 1.0f;
    float _ays = 1.0f;
    float _azs = 1.0f;
    // magnetometer bias and scale factor estimation
    uint16_t _maxCounts = 1000;
    float _deltaThresh = 0.3f;
    uint8_t _coeff = 8;
    uint16_t _counter = 0;
    float _framedelta = 0.0f;
    float _delta   = 0.0f;
    float _hxfilt  = 0.0f;
    float _hyfilt  = 0.0f;
    float _hzfilt  = 0.0f;
    float _hxmax   = 0.0f;
    float _hymax   = 0.0f;
    float _hzmax   = 0.0f;
    float _hxmin   = 0.0f;
    float _hymin   = 0.0f;
    float _hzmin   = 0.0f;
    float _hxb     = 0.0f;
    float _hyb     = 0.0f;
    float _hzb     = 0.0f;
    float _hxs = 1.0f;
    float _hys = 1.0f;
    float _hzs = 1.0f;
    float _avgs;
    // transformation matrix
    /* transform the accel and gyro axes to match the magnetometer axes */
    const int16_t tX[3] = {0,  1,  0};
    const int16_t tY[3] = {1,  0,  0};
    const int16_t tZ[3] = {0,  0, -1};
    // constants
    const float G = 9.807f;
    const float _d2r = 3.14159265359f/180.0f;
    // MPU9250 registers
    const uint8_t ACCEL_OUT           = 0x3B;
    const uint8_t GYRO_OUT            = 0x43;
    const uint8_t TEMP_OUT            = 0x41;
    const uint8_t EXT_SENS_DATA_00    = 0x49;
    const uint8_t ACCEL_CONFIG        = 0x1C;
    const uint8_t ACCEL_FS_SEL_2G     = 0x00;
    const uint8_t ACCEL_FS_SEL_4G     = 0x08;
    const uint8_t ACCEL_FS_SEL_8G     = 0x10;
    const uint8_t ACCEL_FS_SEL_16G    = 0x18;
    const uint8_t GYRO_CONFIG         = 0x1B;
    const uint8_t GYRO_FS_SEL_250DPS  = 0x00;
    const uint8_t GYRO_FS_SEL_500DPS  = 0x08;
    const uint8_t GYRO_FS_SEL_1000DPS = 0x10;
    const uint8_t GYRO_FS_SEL_2000DPS = 0x18;
    const uint8_t ACCEL_CONFIG2       = 0x1D;
    const uint8_t ACCEL_DLPF_184      = 0x01;
    const uint8_t ACCEL_DLPF_92       = 0x02;
    const uint8_t ACCEL_DLPF_41       = 0x03;
    const uint8_t ACCEL_DLPF_20       = 0x04;
    const uint8_t ACCEL_DLPF_10       = 0x05;
    const uint8_t ACCEL_DLPF_5        = 0x06;
    const uint8_t CONFIG              = 0x1A;
    const uint8_t GYRO_DLPF_184       = 0x01;
    const uint8_t GYRO_DLPF_92        = 0x02;
    const uint8_t GYRO_DLPF_41        = 0x03;
    const uint8_t GYRO_DLPF_20        = 0x04;
    const uint8_t GYRO_DLPF_10        = 0x05;
    const uint8_t GYRO_DLPF_5         = 0x06;
    const uint8_t SMPDIV              = 0x19;
    const uint8_t INT_PIN_CFG         = 0x37;
    const uint8_t INT_ENABLE          = 0x38;
    const uint8_t INT_DISABLE         = 0x00;
    const uint8_t INT_PULSE_50US      = 0x00;
    const uint8_t INT_WOM_EN          = 0x40;
    const uint8_t INT_RAW_RDY_EN      = 0x01;
    const uint8_t PWR_MGMNT_1         = 0x6B;
    const uint8_t PWR_CYCLE           = 0x20;
    const uint8_t PWR_RESET           = 0x80;
    const uint8_t CLOCK_SEL_PLL       = 0x01;
    const uint8_t PWR_MGMNT_2         = 0x6C;
    const uint8_t SEN_ENABLE          = 0x00;
    const uint8_t DIS_GYRO            = 0x07;
    const uint8_t USER_CTRL           = 0x6A;
    const uint8_t I2C_MST_EN          = 0x20;
    const uint8_t I2C_MST_CLK         = 0x0D;
    const uint8_t I2C_MST_CTRL        = 0x24;
    const uint8_t I2C_SLV0_ADDR       = 0x25;
    const uint8_t I2C_SLV0_REG        = 0x26;
    const uint8_t I2C_SLV0_DO         = 0x63;
    const uint8_t I2C_SLV0_CTRL       = 0x27;
    const uint8_t I2C_SLV0_EN         = 0x80;
    const uint8_t I2C_READ_FLAG       = 0x80;
    const uint8_t MOT_DETECT_CTRL     = 0x69;
    const uint8_t ACCEL_INTEL_EN      = 0x80;
    const uint8_t ACCEL_INTEL_MODE    = 0x40;
    const uint8_t LP_ACCEL_ODR        = 0x1E;
    const uint8_t WOM_THR             = 0x1F;
    const uint8_t WHO_AM_I            = 0x75;
    const uint8_t FIFO_EN             = 0x23;
    const uint8_t FIFO_TEMP           = 0x80;
    const uint8_t FIFO_GYRO           = 0x70;
    const uint8_t FIFO_ACCEL          = 0x08;
    const uint8_t FIFO_MAG            = 0x01;
    const uint8_t FIFO_COUNT          = 0x72;
    const uint8_t FIFO_READ           = 0x74;
    // AK8963 registers
    const uint8_t AK8963_I2C_ADDR  = 0x0C;
    const uint8_t AK8963_HXL       = 0x03;
    const uint8_t AK8963_CNTL1     = 0x0A;
    const uint8_t AK8963_PWR_DOWN  = 0x00;
    const uint8_t AK8963_CNT_MEAS1 = 0x12;
    const uint8_t AK8963_CNT_MEAS2 = 0x16;
    const uint8_t AK8963_FUSE_ROM  = 0x0F;
    const uint8_t AK8963_CNTL2     = 0x0B;
    const uint8_t AK8963_RESET     = 0x01;
    const uint8_t AK8963_ASA       = 0x10;
    const uint8_t AK8963_WHO_AM_I  = 0x00;

protected: // private functions
    int whoAmI();
    int whoAmIAK8963();

    int writeAK8963Register(uint8_t subAddress, uint8_t data);
    int readAK8963Registers(uint8_t subAddress, uint8_t count, uint8_t* dest);

    int writeRegister(uint8_t subAddress, uint8_t data);
    int readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest);
};

} // namespace pimu