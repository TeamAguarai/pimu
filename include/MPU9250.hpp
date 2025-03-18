/* 
MPU9250.h: Este archivo es una fusion de los sistemas creados por ranranff (GitHub)
Creditos: https://github.com/ranranff/mpu9250/
*/

#ifdef VSCODE_INTELLISENSE_SUPPORT
#include "Writer.hpp"
#include "operations.hpp"
#include "I2Cdev.hpp"
#include "LowPass.hpp"
#include "delay.hpp"
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

    MPU9250() {}

    /* starts communication with the MPU-9250 */
    int begin(){
        
        // select clock source to gyro
        if(writeRegister(PWR_MGMNT_1,CLOCK_SEL_PLL) < 0){
            return -1;
        }
        // enable I2C master mode
        if(writeRegister(USER_CTRL,I2C_MST_EN) < 0){
            return -2;
        }
        // set the I2C bus speed to 400 kHz
        if(writeRegister(I2C_MST_CTRL,I2C_MST_CLK) < 0){
            return -3;
        }
        // set AK8963 to Power Down
        writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN);
        // reset the MPU9250
        writeRegister(PWR_MGMNT_1,PWR_RESET);
        // wait for MPU-9250 to come back up
        delay(1);
        // reset the AK8963
        writeAK8963Register(AK8963_CNTL2,AK8963_RESET);
        // select clock source to gyro
        if(writeRegister(PWR_MGMNT_1,CLOCK_SEL_PLL) < 0){
            return -4;
        }
        // check the WHO AM I byte, expected value is 0x71 (decimal 113) or 0x73 (decimal 115)
        if((whoAmI() != 113)&&(whoAmI() != 115)){
            return -5;
        }
        // enable accelerometer and gyro
        if(writeRegister(PWR_MGMNT_2,SEN_ENABLE) < 0){
            return -6;
        }
        // setting accel range to 16G as default
        if(writeRegister(ACCEL_CONFIG,ACCEL_FS_SEL_16G) < 0){
            return -7;
        }
        _accelScale = G * 16.0f/32767.5f; // setting the accel scale to 16G
        _accelRange = ACCEL_RANGE_16G;
        // setting the gyro range to 2000DPS as default
        if(writeRegister(GYRO_CONFIG,GYRO_FS_SEL_2000DPS) < 0){
            return -8;
        }
        _gyroScale = 2000.0f/32767.5f * _d2r; // setting the gyro scale to 2000DPS
        _gyroRange = GYRO_RANGE_2000DPS;
        // setting bandwidth to 184Hz as default
        if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_184) < 0){
            return -9;
        }
        if(writeRegister(CONFIG,GYRO_DLPF_184) < 0){ // setting gyro bandwidth to 184Hz
            return -10;
        }
        _bandwidth = DLPF_BANDWIDTH_184HZ;
        // setting the sample rate divider to 0 as default
        if(writeRegister(SMPDIV,0x00) < 0){
            return -11;
        }
        _srd = 0;
        // enable I2C master mode
        if(writeRegister(USER_CTRL,I2C_MST_EN) < 0){
            return -12;
        }
        // set the I2C bus speed to 400 kHz
        if( writeRegister(I2C_MST_CTRL,I2C_MST_CLK) < 0){
            return -13;
        }
        // check AK8963 WHO AM I register, expected value is 0x48 (decimal 72)
        if( whoAmIAK8963() != 72 ){
            return -14;
        }
        /* get the magnetometer calibration */
        // set AK8963 to Power Down
        if(writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN) < 0){
            return -15;
        }
        delay(100); // long wait between AK8963 mode changes
        // set AK8963 to FUSE ROM access
        if(writeAK8963Register(AK8963_CNTL1,AK8963_FUSE_ROM) < 0){
            return -16;
        }
        delay(100); // long wait between AK8963 mode changes
        // read the AK8963 ASA registers and compute magnetometer scale factors
        readAK8963Registers(AK8963_ASA,3,_buffer);
        _magScaleX = ((((float)_buffer[0]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
        _magScaleY = ((((float)_buffer[1]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
        _magScaleZ = ((((float)_buffer[2]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
        // std::cout<<__FILE__<<__LINE__<<"  "<<_magScaleX<<"\t"<<_magScaleY<<"\t"<<_magScaleZ<<"\n";
        // set AK8963 to Power Down
        if(writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN) < 0){
            return -17;
        }
        delay(100); // long wait between AK8963 mode changes
        // set AK8963 to 16 bit resolution, 100 Hz update rate
        if(writeAK8963Register(AK8963_CNTL1,AK8963_CNT_MEAS2) < 0){
            return -18;
        }
        delay(100); // long wait between AK8963 mode changes
        // select clock source to gyro
        if(writeRegister(PWR_MGMNT_1,CLOCK_SEL_PLL) < 0){
            return -19;
        }
        // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
        readAK8963Registers(AK8963_HXL,7,_buffer);
    
    
        // set gyro and accel range, bandwidth, and srd
        if (setGyroRange(GYRO_RANGE_250DPS) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -20;
        }
        if (setAccelRange(ACCEL_RANGE_2G) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -21;
        }
        if (setDlpfBandwidth(DLPF_BANDWIDTH_20HZ) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -22;
        }
        if (setSrd(19) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -23;
        }
        return 1;
    }
    
    /* writes a byte to MPU9250 register given a register address and data */
    int writeRegister(uint8_t subAddress, uint8_t data){
    
        writeByte(_address, subAddress, data);
    
        delay(10); // wiringPi delay
    
        /* read back the register */
        readRegisters(subAddress, 1, _buffer);
        /* check the read back register against the written register */
    
        if(_buffer[0] == data) {
            return 1;
        }
        else{
            return -1;
        }
    }
    
    /* reads registers from MPU9250 given a starting register address, number of bytes, and a pointer to store data */
    int readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest){
        if ( count == readBytes(_address, subAddress, count, dest)){
            return 0;
        }
        else {
            return -1;
        }
    
    }
    
    /* sets the accelerometer full scale range to values other than default */
    int setAccelRange(AccelRange range) {
        // use low speed SPI for register setting
        _useSPIHS = false;
        switch(range) {
        case ACCEL_RANGE_2G: {
            std::cout << "accel range 2g\n";
            // setting the accel range to 2G
            if(writeRegister(ACCEL_CONFIG,ACCEL_FS_SEL_2G) < 0){
                return -1;
            }
            _accelScale = G * 2.0f/32767.5f; // setting the accel scale to 2G
            break;
        }
        case ACCEL_RANGE_4G: {
            // setting the accel range to 4G
            if(writeRegister(ACCEL_CONFIG,ACCEL_FS_SEL_4G) < 0){
                return -1;
            }
            _accelScale = G * 4.0f/32767.5f; // setting the accel scale to 4G
            break;
        }
        case ACCEL_RANGE_8G: {
            // setting the accel range to 8G
            if(writeRegister(ACCEL_CONFIG,ACCEL_FS_SEL_8G) < 0){
                return -1;
            }
            _accelScale = G * 8.0f/32767.5f; // setting the accel scale to 8G
            break;
        }
        case ACCEL_RANGE_16G: {
            // setting the accel range to 16G
            if(writeRegister(ACCEL_CONFIG,ACCEL_FS_SEL_16G) < 0){
                return -1;
            }
            _accelScale = G * 16.0f/32767.5f; // setting the accel scale to 16G
            break;
        }
        }
        _accelRange = range;
        return 1;
    }
    
    /* sets the gyro full scale range to values other than default */
    int setGyroRange(GyroRange range) {
        // use low speed SPI for register setting
        _useSPIHS = false;
        switch(range) {
        case GYRO_RANGE_250DPS: {
            // setting the gyro range to 250DPS
            if(writeRegister(GYRO_CONFIG,GYRO_FS_SEL_250DPS) < 0){
                return -1;
            }
            _gyroScale = 250.0f/32767.5f * _d2r; // setting the gyro scale to 250DPS
            break;
        }
        case GYRO_RANGE_500DPS: {
            // setting the gyro range to 500DPS
            if(writeRegister(GYRO_CONFIG,GYRO_FS_SEL_500DPS) < 0){
                return -1;
            }
            _gyroScale = 500.0f/32767.5f * _d2r; // setting the gyro scale to 500DPS
            break;
        }
        case GYRO_RANGE_1000DPS: {
            // setting the gyro range to 1000DPS
            if(writeRegister(GYRO_CONFIG,GYRO_FS_SEL_1000DPS) < 0){
                return -1;
            }
            _gyroScale = 1000.0f/32767.5f * _d2r; // setting the gyro scale to 1000DPS
            break;
        }
        case GYRO_RANGE_2000DPS: {
            // setting the gyro range to 2000DPS
            if(writeRegister(GYRO_CONFIG,GYRO_FS_SEL_2000DPS) < 0){
                return -1;
            }
            _gyroScale = 2000.0f/32767.5f * _d2r; // setting the gyro scale to 2000DPS
            break;
        }
        }
        _gyroRange = range;
        return 1;
    }
    
    /* sets the DLPF bandwidth to values other than default */
    int setDlpfBandwidth(DlpfBandwidth bandwidth) {
        // use low speed SPI for register setting
        _useSPIHS = false;
        switch(bandwidth) {
        case DLPF_BANDWIDTH_184HZ: {
            if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_184) < 0){ // setting accel bandwidth to 184Hz
                return -1;
            }
            if(writeRegister(CONFIG,GYRO_DLPF_184) < 0){ // setting gyro bandwidth to 184Hz
                return -2;
            }
            break;
        }
        case DLPF_BANDWIDTH_92HZ: {
            if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_92) < 0){ // setting accel bandwidth to 92Hz
                return -1;
            }
            if(writeRegister(CONFIG,GYRO_DLPF_92) < 0){ // setting gyro bandwidth to 92Hz
                return -2;
            }
            break;
        }
        case DLPF_BANDWIDTH_41HZ: {
            if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_41) < 0){ // setting accel bandwidth to 41Hz
                return -1;
            }
            if(writeRegister(CONFIG,GYRO_DLPF_41) < 0){ // setting gyro bandwidth to 41Hz
                return -2;
            }
            break;
        }
        case DLPF_BANDWIDTH_20HZ: {
            if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_20) < 0){ // setting accel bandwidth to 20Hz
                return -1;
            }
            if(writeRegister(CONFIG,GYRO_DLPF_20) < 0){ // setting gyro bandwidth to 20Hz
                return -2;
            }
            break;
        }
        case DLPF_BANDWIDTH_10HZ: {
            if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_10) < 0){ // setting accel bandwidth to 10Hz
                return -1;
            }
            if(writeRegister(CONFIG,GYRO_DLPF_10) < 0){ // setting gyro bandwidth to 10Hz
                return -2;
            }
            break;
        }
        case DLPF_BANDWIDTH_5HZ: {
            if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_5) < 0){ // setting accel bandwidth to 5Hz
                return -1;
            }
            if(writeRegister(CONFIG,GYRO_DLPF_5) < 0){ // setting gyro bandwidth to 5Hz
                return -2;
            }
            break;
        }
        }
        _bandwidth = bandwidth;
        return 1;
    }
    
    /* sets the sample rate divider to values other than default */
    int setSrd(uint8_t srd) {
        // use low speed SPI for register setting
        _useSPIHS = false;
        /* setting the sample rate divider to 19 to facilitate setting up magnetometer */
        if(writeRegister(SMPDIV,19) < 0){ // setting the sample rate divider
            return -1;
        }
        if(srd > 9){
            // set AK8963 to Power Down
            if(writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN) < 0){
                std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
                return -2;
            }
            delay(100); // long wait between AK8963 mode changes
            // set AK8963 to 16 bit resolution, 8 Hz update rate
            if(writeAK8963Register(AK8963_CNTL1,AK8963_CNT_MEAS1) < 0){
                std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
                return -3;
            }
            delay(100); // long wait between AK8963 mode changes
            // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
            readAK8963Registers(AK8963_HXL,7,_buffer);
        } else {
            // set AK8963 to Power Down
            if(writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN) < 0){
                std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
                return -2;
            }
            delay(100); // long wait between AK8963 mode changes
            // set AK8963 to 16 bit resolution, 100 Hz update rate
            if(writeAK8963Register(AK8963_CNTL1,AK8963_CNT_MEAS2) < 0){
                std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
                return -3;
            }
            delay(100); // long wait between AK8963 mode changes
            // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
            readAK8963Registers(AK8963_HXL,7,_buffer);
        }
        /* setting the sample rate divider */
        if(writeRegister(SMPDIV,srd) < 0){ // setting the sample rate divider
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -4;
        }
        _srd = srd;
        return 1;
    }
    
    /* enables the data ready interrupt */
    int enableDataReadyInterrupt() {
        // use low speed SPI for register setting
        _useSPIHS = false;
        /* setting the interrupt */
        if (writeRegister(INT_PIN_CFG,INT_PULSE_50US) < 0){ // setup interrupt, 50 us pulse
            return -1;
        }
        if (writeRegister(INT_ENABLE,INT_RAW_RDY_EN) < 0){ // set to data ready
            return -2;
        }
        return 1;
    }
    
    /* disables the data ready interrupt */
    int disableDataReadyInterrupt() {
        // use low speed SPI for register setting
        _useSPIHS = false;
        if(writeRegister(INT_ENABLE,INT_DISABLE) < 0){ // disable interrupt
            return -1;
        }
        return 1;
    }
    
    /* configures and enables wake on motion, low power mode */
    int enableWakeOnMotion(float womThresh_mg,LpAccelOdr odr) {
        // use low speed SPI for register setting
        _useSPIHS = false;
        // set AK8963 to Power Down
        writeAK8963Register(AK8963_CNTL1,AK8963_PWR_DOWN);
        // reset the MPU9250
        writeRegister(PWR_MGMNT_1,PWR_RESET);
        // wait for MPU-9250 to come back up
        delay(1);
        if(writeRegister(PWR_MGMNT_1,0x00) < 0){ // cycle 0, sleep 0, standby 0
            return -1;
        }
        if(writeRegister(PWR_MGMNT_2,DIS_GYRO) < 0){ // disable gyro measurements
            return -2;
        }
        if(writeRegister(ACCEL_CONFIG2,ACCEL_DLPF_184) < 0){ // setting accel bandwidth to 184Hz
            return -3;
        }
        if(writeRegister(INT_ENABLE,INT_WOM_EN) < 0){ // enabling interrupt to wake on motion
            return -4;
        }
        if(writeRegister(MOT_DETECT_CTRL,(ACCEL_INTEL_EN | ACCEL_INTEL_MODE)) < 0){ // enabling accel hardware intelligence
            return -5;
        }
        _womThreshold = remap(womThresh_mg, 0.0f, 1020.0f, 0.0f, 255.0f);
        if(writeRegister(WOM_THR,_womThreshold) < 0){ // setting wake on motion threshold
            return -6;
        }
        if(writeRegister(LP_ACCEL_ODR,(uint8_t)odr) < 0){ // set frequency of wakeup
            return -7;
        }
        if(writeRegister(PWR_MGMNT_1,PWR_CYCLE) < 0){ // switch to accel low power mode
            return -8;
        }
        return 1;
    }
    
    /* reads the most current data from MPU9250 and stores in buffer */
    int readSensor() {
        _useSPIHS = true; // use the high speed SPI for data readout
        // grab the data from the MPU9250
        if (readRegisters(ACCEL_OUT, 21, _buffer) < 0) {
            return -1;
        }
        // combine into 16 bit values
        _axcounts = (((int16_t)_buffer[0]) << 8) | _buffer[1];
        _aycounts = (((int16_t)_buffer[2]) << 8) | _buffer[3];
        _azcounts = (((int16_t)_buffer[4]) << 8) | _buffer[5];
        _tcounts  = (((int16_t)_buffer[6]) << 8) | _buffer[7];
        _gxcounts = (((int16_t)_buffer[8]) << 8) | _buffer[9];
        _gycounts = (((int16_t)_buffer[10]) << 8) | _buffer[11];
        _gzcounts = (((int16_t)_buffer[12]) << 8) | _buffer[13];
        _hxcounts = (((int16_t)_buffer[15]) << 8) | _buffer[14];
        _hycounts = (((int16_t)_buffer[17]) << 8) | _buffer[16];
        _hzcounts = (((int16_t)_buffer[19]) << 8) | _buffer[18];
    
        // transform and convert to float values
        _ax = ((float)(tX[0]*_axcounts + tX[1]*_aycounts + tX[2]*_azcounts) * _accelScale);
        _ay = ((float)(tY[0]*_axcounts + tY[1]*_aycounts + tY[2]*_azcounts) * _accelScale);
        _az = ((float)(tZ[0]*_axcounts + tZ[1]*_aycounts + tZ[2]*_azcounts) * _accelScale);
        _gx = ((float)(tX[0]*_gxcounts + tX[1]*_gycounts + tX[2]*_gzcounts) * _gyroScale); 
        _gy = ((float)(tY[0]*_gxcounts + tY[1]*_gycounts + tY[2]*_gzcounts) * _gyroScale); 
        _gz = ((float)(tZ[0]*_gxcounts + tZ[1]*_gycounts + tZ[2]*_gzcounts) * _gyroScale);
        _hx = (((float)(_hxcounts) * _magScaleX) - _hxb)*_hxs;
        _hy = (((float)(_hycounts) * _magScaleY) - _hyb)*_hys;
        _hz = (((float)(_hzcounts) * _magScaleZ) - _hzb)*_hzs;
        _t = ((((float) _tcounts) - _tempOffset)/_tempScale) + _tempOffset;
        return 1;
    }
    
    /* returns the gyroscope measurement in the x direction, rad/s */
    float getGyroX_rads() {
        return _gx;
    }
    
    /* returns the gyroscope measurement in the y direction, rad/s */
    float getGyroY_rads() {
        return _gy;
    }
    
    /* returns the gyroscope measurement in the z direction, rad/s */
    float getGyroZ_rads() {
        return _gz;
    }
    
    /* returns the accelerometer measurement in the x direction, m/s/s */
    float getAccelX_mss() {
        return _ax;
    }
    
    /* returns the accelerometer measurement in the y direction, m/s/s */
    float getAccelY_mss() {
        return _ay;
    }
    
    /* returns the accelerometer measurement in the z direction, m/s/s */
    float getAccelZ_mss() {
        return _az;
    }
    
    /* returns the magnetometer measurement in the x direction, uT */
    float getMagX_uT() {
        return _hx;
    }
    
    /* returns the magnetometer measurement in the y direction, uT */
    float getMagY_uT() {
        return _hy;
    }
    
    /* returns the magnetometer measurement in the z direction, uT */
    float getMagZ_uT() {
        return _hz;
    }
    
    /* returns the die temperature, C */
    float getTemperature_C() {
        return _t;
    }
    
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
    // track success of interacting with Sensor
    int _status = 0;
    // buffer for reading from Sensor
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
    /* gets the MPU9250 WHO_AM_I register value, expected to be 0x71 */
    int whoAmI(){
        // read the WHO AM I register
        if (readRegisters(WHO_AM_I,1,_buffer) < 0) {
            return -1;
        }
        // return the register value
        return _buffer[0];
    }
    
    /* gets the AK8963 WHO_AM_I register value, expected to be 0x48 */
    int whoAmIAK8963(){
        // read the WHO AM I register
        if (readAK8963Registers(AK8963_WHO_AM_I,1,_buffer) < 0) {
            return -1;
        }
        // return the register value
        return _buffer[0];
    }
    
    /* writes a register to the AK8963 given a register address and data */
    int writeAK8963Register(uint8_t subAddress, uint8_t data){
        // set slave 0 to the AK8963 and set for write
        if (writeRegister(I2C_SLV0_ADDR,AK8963_I2C_ADDR) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -1;
        }
        // set the register to the desired AK8963 sub address
        if (writeRegister(I2C_SLV0_REG,subAddress) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -2;
        }
        // store the data for write
        if (writeRegister(I2C_SLV0_DO,data) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -3;
        }
        // enable I2C and send 1 byte
        if (writeRegister(I2C_SLV0_CTRL,I2C_SLV0_EN | (uint8_t)1) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -4;
        }
    
        // read the register and confirm
        if (readAK8963Registers(subAddress,1,_buffer) < 0) {
            std::cerr<<__FILE__<<__LINE__<<": error."<<std::endl;
            return -5;
        }
    
        if(_buffer[0] == data) {
            return 1;
        } else{
            return -6;
        }
    }
    
    /* reads registers from the AK8963 */
    int readAK8963Registers(uint8_t subAddress, uint8_t count, uint8_t* dest){
        // set slave 0 to the AK8963 and set for read
        if (writeRegister(I2C_SLV0_ADDR,AK8963_I2C_ADDR | I2C_READ_FLAG) < 0) {
            return -1;
        }
        // set the register to the desired AK8963 sub address
        if (writeRegister(I2C_SLV0_REG,subAddress) < 0) {
            return -2;
        }
        // enable I2C and request the bytes
        if (writeRegister(I2C_SLV0_CTRL,I2C_SLV0_EN | count) < 0) {
            return -3;
        }
        delay(1); // takes some time for these registers to fill
        // read the bytes off the MPU9250 EXT_SENS_DATA registers
        _status = readRegisters(EXT_SENS_DATA_00,count,dest);
        return _status;
    }

};





} // namespace pimu