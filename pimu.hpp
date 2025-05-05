/* File: pimu.1.0.0.hpp
Created by: maino
Date: 2025-05-05 12:50:54
OS: Windows 11
Folder: include

Merged Files:
delay.hpp
I2Cdev.hpp
Writer.hpp
operations.hpp
LowPass.hpp
MPU9250.hpp
LinearRegression.hpp
type.hpp
Accel.hpp
Gyro.hpp
Imu.hpp
*/

// ===== delay.hpp =====
#include <unistd.h>

namespace pimu {

/* stops the program execution for a duration in miliseconds */
void delay(int duration_miliseconds){
    while(duration_miliseconds--){
        usleep(1000);
    }
}
} // namespace pimu

// ===== I2Cdev.hpp =====
// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//     2012-06-09 - fix major issue with reading > 32 bytes at a time with Arduino Wire
//                - add compiler warnings when using outdated or IDE or limited I2Cdev implementation
//     2011-11-01 - fix write*Bits mask calculation (thanks sasquatch @ Arduino forums)
//     2011-10-03 - added automatic Arduino version detection for ease of use
//     2011-10-02 - added Gene Knight's NBWire TwoWire class implementation with small modifications
//     2011-08-31 - added support for Arduino 1.0 Wire library (methods are different from 0.x)
//     2011-08-03 - added optional timeout parameter to read* methods to easily change from default
//     2011-08-02 - added support for 16-bit registers
//                - fixed incorrect Doxygen comments on some methods
//                - added timeout value for read operations (thanks mem @ Arduino forums)
//     2011-07-30 - changed read/write function structures to return success or byte counts
//                - made all methods static for multi-device memory savings
//     2011-07-28 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#ifdef linux
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define I2C_OK 0
#define I2C_ERR -1

namespace pimu {
    
int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data);
int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data);
int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

int writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
int writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
int writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
int writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
int writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
int writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
int writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);


    
/** Default timeout value for read operations.
 * Set this to 0 to disable timeout detection.
 */
uint16_t readTimeout = 0;
/** Default constructor.
 */

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read a single bit from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data) {
    uint16_t b;
    uint8_t count = readWord(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = readByte(devAddr, regAddr, &b)) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read multiple bits from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-15)
 * @param length Number of bits to read (not more than 16)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
 */
int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data) {
    // 1101011001101001 read byte
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    //    010           masked
    //           -> 010 shifted
    uint8_t count;
    uint16_t w;
    if ((count = readWord(devAddr, regAddr, &w)) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        w &= mask;
        w >>= (bitStart - length + 1);
        *data = w;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    return readBytes(devAddr, regAddr, 1, data);
}

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data) {
    return readWords(devAddr, regAddr, 1, data);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
    int8_t count = 0;
#ifdef DEBUG
printf("read %#x %#x %u\n",devAddr,regAddr,length);
#endif
int fd = open("/dev/i2c-1", O_RDWR);

if (fd < 0) {
    fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
    return(-1);
}
#ifdef linux
if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
    fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
    close(fd);
    return(-1);
}
#endif
if (write(fd, &regAddr, 1) != 1) {
    fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
    close(fd);
    return(-1);
}
count = read(fd, data, length);
if (count < 0) {
    fprintf(stderr, "Failed to read device(%d): %s\n", count, strerror(errno));
    close(fd);
    return(-1);
} else if (count != length) {
    fprintf(stderr, "Short read  from device, expected %d, got %d\n", length, count);
    close(fd);
    return(-1);
}
close(fd);

return count;
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (0 indicates failure)
 */
int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) {
    int8_t count = 0;
    
    printf("ReadWords() not implemented\n");
    // Use readBytes() and potential byteswap
    *data = 0; // keep the compiler quiet
    
    return count;
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
int writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(devAddr, regAddr, b);
}

/** write a single bit in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
int writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data) {
    uint16_t w;
    readWord(devAddr, regAddr, &w);
    w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
    return writeWord(devAddr, regAddr, w);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
int writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (readByte(devAddr, regAddr, &b) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return writeByte(devAddr, regAddr, b);
    } else {
        return -1;
    }
}

/** Write multiple bits in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-15)
 * @param length Number of bits to write (not more than 16)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
int writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) {
    //              010 value to write
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    // 0001110000000000 mask byte
    // 1010111110010110 original value (sample)
    // 1010001110010110 original & ~mask
    // 1010101110010110 masked | value
    uint16_t w;
    if (readWord(devAddr, regAddr, &w) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        w &= ~(mask); // zero all important bits in existing word
        w |= data; // combine data with existing word
        return writeWord(devAddr, regAddr, w);
    } else {
        return -1;
    }
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
int writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return writeBytes(devAddr, regAddr, 1, &data);
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
int writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
    return writeWords(devAddr, regAddr, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
int writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    int8_t count = 0;
    uint8_t buf[128];
    int fd;
    
    #ifdef DEBUG
    printf("write %#x %#x\n",devAddr,regAddr);
    #endif
    if (length > 127) {
        fprintf(stderr, "Byte write count (%d) > 127\n", length);
        return -1;
    }
    
    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return -1;
    }
    #ifdef linux
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
        fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    #endif
    buf[0] = regAddr;
    memcpy(buf+1,data,length);
    count = write(fd, buf, length+1);
    if (count < 0) {
        fprintf(stderr, "Failed to write device(%d): %s\n", count, strerror(errno));
        close(fd);
        return -1;
    } else if (count != length+1) {
        fprintf(stderr, "Short write to device, expected %d, got %d\n", length+1, count);
        close(fd);
        return -1;
    }
    close(fd);
    
    return 0;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
int writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {
    int8_t count = 0;
    uint8_t buf[128];
    int i, fd;
    
    // Should do potential byteswap and call writeBytes() really, but that
    // messes with the callers buffer
    
    if (length > 63) {
        fprintf(stderr, "Word write count (%d) > 63\n", length);
        return -1;
    }
    
    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return -1;
    }
    #ifdef linux
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
        fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    #endif
    buf[0] = regAddr;
    for (i = 0; i < length; i++) {
        buf[i*2+1] = data[i] >> 8;
        buf[i*2+2] = data[i];
    }
    count = write(fd, buf, length*2+1);
    if (count < 0) {
        fprintf(stderr, "Failed to write device(%d): %s\n", count, strerror(errno));
        close(fd);
        return -1;
    } else if (count != length*2+1) {
        fprintf(stderr, "Short write to device, expected %d, got %d\n", length+1, count);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
} 

} // namespace pimu

// ===== Writer.hpp =====
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

namespace pimu {

class Writer {
private:
    std::ofstream file_;     
    bool is_open_;           
    std::string delimiter_;  

public:
    Writer(std::string file_name, std::string header, std::string delim);
    ~Writer();

    void write_row(const std::vector<std::string>& data);
    void write_line(const std::string& line);
    void close();
};

/* Constructor: abre el archivo y escribe el encabezado si es posible */
Writer::Writer(std::string file_name, std::string header, std::string delim) 
    : is_open_(false), delimiter_(delim) {
    file_.open(file_name);
    if (file_.is_open()) {
        is_open_ = true;
        file_ << header << "\n";
    } else {
        std::cerr << "Error: No se pudo abrir el archivo " << file_name << "\n";
    }
}

/* Destructor: cierra el archivo si está abierto */
Writer::~Writer() {
    if (is_open_) file_.close();
}

/* Escribe una fila en el archivo separando los valores con el delimitador */
void Writer::write_row(const std::vector<std::string>& data) {
    if (!is_open_) {
        std::cerr << "Error: El archivo no está abierto.\n";
        return;
    } 
    std::ostringstream oss;
    for (size_t i = 0; i < data.size(); ++i) {
        oss << data[i];
        if (i != data.size() - 1) {
            oss << delimiter_;
        }
    }
    file_ << oss.str() << "\n";
}

/* Escribe una línea en el archivo */
void Writer::write_line(const std::string& line) {
    if (!is_open_) {
        std::cerr << "Error: El archivo no está abierto.\n";
        return;
    }
    file_ << line << "\n";
}

/* Cierra el archivo si está abierto */
void Writer::close() {
    if (is_open_) {
        file_.close();
        is_open_ = false;
    }
}

} // namespace pimu


// ===== operations.hpp =====
#include <algorithm>
#include <chrono>
#include <cmath>

namespace pimu {

// Referencia: https://stackoverflow.com/questions/70221264/how-do-i-set-the-precision-of-a-float-variable-in-c
float round(float num, int decimals);
double round(double num, int decimals);
int round(int num, int decimals); // No afecta enteros, solo los devuelve.

// Referencia: https://stackoverflow.com/questions/8684327/c-map-number-ranges
template <typename T>
T remap(T value, T in_min, T in_max, T out_min, T out_max) {
    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
}

// Redondeo para float
float round(float num, int decimals) {
    float factor = std::pow(10.0f, decimals);
    return std::round(std::round(num * (factor * 10.0f)) / 10.0f) / factor;
}

// Redondeo para double
double round(double num, int decimals) {
    double factor = std::pow(10.0, decimals);
    return std::round(std::round(num * (factor * 10.0)) / 10.0) / factor;
}

// Redondeo para enteros (No afecta valores enteros)
int round(int num, int) {
    return num;
}

} 

// ===== LowPass.hpp =====
#include <stdexcept>

namespace pimu {

template<typename T>
class LowPass {
private:
    T _alpha;      
    T prev_output_;  
    bool alpha_defined_ = false;

public:
    LowPass();

    void setAlpha(T value);
    void setInitialValue(T value);
    bool isAlphaDefined();
    T filter(T input);
};

/* Constructor: define el tipo de dato que se usará */
template<typename T>
LowPass<T>::LowPass() : prev_output_(static_cast<T>(0)) {}

/* Establece el coeficiente de suavizado */
template<typename T>
void LowPass<T>::setAlpha(T value) {
    if (value < static_cast<T>(0) || value > static_cast<T>(1)) {
        throw std::invalid_argument("The smoothing coefficient must be in the range [0,1].");
    }
    this->alpha_defined_ = true;
    this->_alpha = value;
}

/* Establece el valor inicial del filtro */
template<typename T>
void LowPass<T>::setInitialValue(T value) {
    this->prev_output_ = value;
}

/* Verifica si el coeficiente alpha ha sido definido */
template<typename T>
bool LowPass<T>::isAlphaDefined() {
    return this->alpha_defined_;
}

/* Aplica el filtro pasa-bajos al valor de entrada */
template<typename T>
T LowPass<T>::filter(T input) {
    if (!this->alpha_defined_) {
        throw std::runtime_error("The smoothing coefficient has not been initialized or has an invalid value.");
    }

    T output = this->_alpha * input + (static_cast<T>(1) - this->_alpha) * this->prev_output_;
    this->prev_output_ = output;
    return output;
}

} // namespace pimu


// ===== MPU9250.hpp =====
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

    int begin();
    int writeRegister(uint8_t subAddress, uint8_t data);
    int readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest);

    int setAccelRange(AccelRange range);
    int setGyroRange(GyroRange range);
    int setDlpfBandwidth(DlpfBandwidth bandwidth);
    int setSrd(uint8_t srd);

    int enableDataReadyInterrupt();
    int disableDataReadyInterrupt();
    int enableWakeOnMotion(float womThresh_mg,LpAccelOdr odr);
    
    int readSensor();
    float getGyroX_rads();
    float getGyroY_rads();
    float getGyroZ_rads();
    float getAccelX_mss();
    float getAccelY_mss();
    float getAccelZ_mss();
    float getMagX_uT();
    float getMagY_uT();
    float getMagZ_uT();
    float getTemperature_C();
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


/* starts communication with the MPU-9250 */
int MPU9250::begin(){
    
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
int MPU9250::writeRegister(uint8_t subAddress, uint8_t data){

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
int MPU9250::readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest){
    if ( count == readBytes(_address, subAddress, count, dest)){
        return 0;
    }
    else {
        return -1;
    }

}

/* sets the accelerometer full scale range to values other than default */
int MPU9250::setAccelRange(AccelRange range) {
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
int MPU9250::setGyroRange(GyroRange range) {
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
int MPU9250::setDlpfBandwidth(DlpfBandwidth bandwidth) {
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
int MPU9250::setSrd(uint8_t srd) {
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
int MPU9250::enableDataReadyInterrupt() {
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
int MPU9250::disableDataReadyInterrupt() {
    // use low speed SPI for register setting
    _useSPIHS = false;
    if(writeRegister(INT_ENABLE,INT_DISABLE) < 0){ // disable interrupt
        return -1;
    }
    return 1;
}

/* configures and enables wake on motion, low power mode */
int MPU9250::enableWakeOnMotion(float womThresh_mg,LpAccelOdr odr) {
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
int MPU9250::readSensor() {
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
float MPU9250::getGyroX_rads() {
    return _gx;
}

/* returns the gyroscope measurement in the y direction, rad/s */
float MPU9250::getGyroY_rads() {
    return _gy;
}

/* returns the gyroscope measurement in the z direction, rad/s */
float MPU9250::getGyroZ_rads() {
    return _gz;
}

/* returns the accelerometer measurement in the x direction, m/s/s */
float MPU9250::getAccelX_mss() {
    return _ax;
}

/* returns the accelerometer measurement in the y direction, m/s/s */
float MPU9250::getAccelY_mss() {
    return _ay;
}

/* returns the accelerometer measurement in the z direction, m/s/s */
float MPU9250::getAccelZ_mss() {
    return _az;
}

/* returns the magnetometer measurement in the x direction, uT */
float MPU9250::getMagX_uT() {
    return _hx;
}

/* returns the magnetometer measurement in the y direction, uT */
float MPU9250::getMagY_uT() {
    return _hy;
}

/* returns the magnetometer measurement in the z direction, uT */
float MPU9250::getMagZ_uT() {
    return _hz;
}

/* returns the die temperature, C */
float MPU9250::getTemperature_C() {
    return _t;
}



} // namespace pimu

// ===== LinearRegression.hpp =====
#include <iostream>

namespace pimu {
    
class LinearRegression {
private:
    int num_points_ = 0;      
    float x_sum_ = 0.0f;        
    float y_sum_ = 0.0f;        
    float x_squared_sum_ = 0.0f; 
    float x_times_y_sum_ = 0.0f;  
    float slope = 0.0f;       
    float intercept = 0.0f;   
    bool computed = false;      

public:
    LinearRegression();

    void addDataPoint(float x, float y);
    void zero();
    void computeCoefficients();
    void setCoefficients(float slope, float intercept);
    float getSlope() const;
    float getIntercept() const;
    float predict(float x) const;
    void printEquation() const;
};

/* Constructor */
LinearRegression::LinearRegression() {
    zero();
}

/* Adds a new data point (x, y) and updates the accumulated sums */
void LinearRegression::addDataPoint(float x, float y) {
    num_points_++;
    x_sum_ += x;
    y_sum_ += y;
    x_squared_sum_ += (x * x);
    x_times_y_sum_ += (x * y);
    computed = false;  // New data requires re-computation of m and b
}

/* Sets all variables to zero. Resets class */
void LinearRegression::zero() {
    num_points_ = 0;      
    x_sum_ = 0.0;        
    y_sum_ = 0.0;        
    x_squared_sum_ = 0.0; 
    x_times_y_sum_ = 0.0;  
    slope = 0.0;       
    intercept = 0.0;   
    computed = false;      
}

/* Computes the regression coefficients m (slope) and b (intercept) */
void LinearRegression::computeCoefficients() {
    if (num_points_ < 2) {
        std::cerr << "Error: At least 2 points are needed to compute linear regression." << std::endl;
        return;
    }

    float denominator = (num_points_ * x_squared_sum_ - (x_sum_ * x_sum_));
    if (denominator == 0) {
        std::cerr << "Error: Division by zero when computing regression." << std::endl;
        return;
    }

    slope = (num_points_ * x_times_y_sum_ - x_sum_ * y_sum_) / denominator;
    intercept = (y_sum_ - slope * x_sum_) / num_points_;
    computed = true;
}

/* Sets the slope (m) and intercept (b) */
void LinearRegression::setCoefficients(float new_slope, float new_intercept) {
    slope = new_slope;
    intercept = new_intercept;
    computed = true;
}

/* Returns the slope (m) */
float LinearRegression::getSlope() const {
    return computed ? slope : 0;
}

/* Returns the intercept (b) */
float LinearRegression::getIntercept() const {
    return computed ? intercept : 0;
}

/* Predicts y for a given x using the regression equation */
float LinearRegression::predict(float x) const {
    return computed ? (slope * x + intercept) : 0;
}

/* Prints the equation of the regression line */
void LinearRegression::printEquation() const {
    if (computed) {
        std::cout << "Regression equation: y = " << slope << "x + " << intercept << std::endl;
    } else {
        std::cerr << "Error: Coefficients not computed. Call computeCoefficients() first." << std::endl;
    }
}          

} // namespace pimu


// ===== type.hpp =====
namespace pimu
{

/* three axis sensor data template (gyro, accel or mag) */
struct Sensor
{
    /* Sensor axis */
    float x, y, z;
};

/* mpu module data template (gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z) */
struct MultiSensor 
{
    /* gyro data */
    float gx, gy, gz; 

    /* accel data */
    float ax, ay, az;
};

}// namespace pimu

// ===== Accel.hpp =====
#ifdef VSCODE_INTELLISENSE_SUPPORT
#include "LowPass.hpp"
#include "MPU9250.hpp"
#include "type.hpp"
#include "LinearRegression.hpp"
#endif

#include <chrono>
#include <iostream>
#include <cmath>

namespace pimu {

class Accel {
public:
    explicit Accel(MPU9250 &module);

    int calibrate(int duration_seconds);
    Sensor read();
    void print(Sensor read_data);
    void updateAngles();
    float getXAxisAngle();
    float getYAxisAngle();
    float getXBias();
    float getYBias();
    float getZBias();

private:
    MPU9250 &module_;

    float x_axis_angle_ = 0.0f;
    float y_axis_angle_ = 0.0f;

    int calibration_num_samples_ = 0;
    bool calibrated_ = false;

    float x_bias_ = 0.0f;
    float y_bias_ = 0.0f;
    float z_bias_ = 0.0f;

    const float kG_ = 9.807f;
};

Accel::Accel(MPU9250 &module) : module_(module) {}

/* calibrate accel offsets */
int Accel::calibrate(int duration_seconds) {
    float ax_sum = 0.0f, ay_sum = 0.0f, az_sum = 0.0f;
    calibration_num_samples_ = 0;
    x_bias_ = y_bias_ = z_bias_ = 0;

    // take samples and find bias
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::seconds(duration_seconds);
    while (std::chrono::high_resolution_clock::now() < end) {
        calibration_num_samples_++;

        module_.readSensor();
        ax_sum += module_.getAccelX_mss() / kG_;
        ay_sum += module_.getAccelY_mss() / kG_;
        az_sum += module_.getAccelZ_mss() / kG_;

        delay(20);
    }

    // set offsets
    x_bias_ = ax_sum / calibration_num_samples_;
    y_bias_ = ay_sum / calibration_num_samples_;
    z_bias_ = az_sum / calibration_num_samples_;

    return 1;
}

/* returns accelerometer readings [G] */
Sensor Accel::read() {
    Sensor return_data;
    module_.readSensor();

    // convert m/s/s to kG_ minus offset
    return_data.x = round((module_.getAccelX_mss() / kG_) - x_bias_, 2);
    return_data.y = round((module_.getAccelY_mss() / kG_) - y_bias_, 2);
    return_data.z = round((module_.getAccelZ_mss() / kG_) - z_bias_, 2);

    return return_data;
}

/* prints in a formatted way the return from Accel::read() */
void Accel::print(Sensor read_data) {
    std::cout << "Accel (x,y,z): " << read_data.x << " G, " 
              << read_data.y << " G, " 
              << read_data.z << " G\n";
}

/* updates angles made from the X and Y axis [sexagesimal degree] */
void Accel::updateAngles() {
    Sensor reading = read();
    x_axis_angle_ = atan(reading.x / sqrt(reading.y * reading.y + reading.z * reading.z));
    y_axis_angle_ = atan(reading.y / sqrt(reading.x * reading.x + reading.z * reading.z));
}

/* returns X axis angle, should be called after Accel::updateAngles() [sexagesimal degree] */
float Accel::getXAxisAngle() { return x_axis_angle_; }

/* returns Y axis angle, should be called after Accel::updateAngles() [sexagesimal degree] */
float Accel::getYAxisAngle() { return y_axis_angle_; }

/* returns X axis offset, 0 by default, only changes after Accel::calibrate() */
float Accel::getXBias() { return x_bias_; }

/* returns Y axis offset, 0 by default, only changes after Accel::calibrate() */
float Accel::getYBias() { return y_bias_; }

/* returns Z axis offset, 0 by default, only changes after Accel::calibrate() */
float Accel::getZBias() { return z_bias_; }

} // namespace pimu


// ===== Gyro.hpp =====
#ifdef VSCODE_INTELLISENSE_SUPPORT
#include "LowPass.hpp"
#include "MPU9250.hpp"
#include "operations.hpp"
#include "type.hpp"
#include "delay.hpp"
#endif

#include <unistd.h>
#include <iostream>
#include <string>
#include <chrono>

namespace pimu {

class Gyro {
public:
    explicit Gyro(MPU9250 &module);

    void setFilterConstant(float constant);
    
    int calibrate(int durationSeconds);

    float getXAxisBias();
    float getYAxisBias();
    float getZAxisBias();
    void setXAxisBias(float bias);
    void setYAxisBias(float bias);
    void setZAxisBias(float bias);

    Sensor read();
    void print(Sensor read_data);

    void updateAngles();
    float getXAxisAngle();
    float getYAxisAngle();

private:
    MPU9250 &module_;

    LowPass<float> x_axis_filter_;
    LowPass<float> y_axis_filter_;
    LowPass<float> z_axis_filter_;

    float x_axis_bias_ = 0.0f;
    float y_axis_bias_ = 0.0f;
    float z_axis_bias_ = 0.0f;

    float x_axis_angle_ = 0.0f;
    float y_axis_angle_ = 0.0f;
    int calibration_num_samples_ = 0; // calibration samples counter

    bool gyro_timer_started_ = false;
    struct timespec gyro_current_time;
    struct timespec gyro_prev_time_;
};

/* pass mpu9250 module as parameter */
Gyro::Gyro(MPU9250 &module) : module_(module), x_axis_filter_(), y_axis_filter_(), z_axis_filter_() {
    this->setFilterConstant(1.0); // low pass filters won't have effect by default
}

/* set low pass filters coefficient values for Gyro::read(), value should be in range [0,1] */
void Gyro::setFilterConstant(float constant) {
    x_axis_filter_.setAlpha(constant);
    y_axis_filter_.setAlpha(constant);
    z_axis_filter_.setAlpha(constant);
}

/* estimates the gyro biases by averaging, run this process for a duration in seconds */
int Gyro::calibrate(int durationSeconds) {
    float gxbD = 0.0f;
    float gzbD = 0.0f;
    float gybD = 0.0f;

    calibration_num_samples_ = 0;

    this->setXAxisBias(0.0f);
    this->setYAxisBias(0.0f);
    this->setZAxisBias(0.0f);

    // take samples and find bias
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::seconds(durationSeconds);
    while (std::chrono::high_resolution_clock::now() < end) {
        calibration_num_samples_++;
        module_.readSensor();
        gxbD += module_.getGyroX_rads();
        gybD += module_.getGyroY_rads();
        gzbD += module_.getGyroZ_rads();
        delay(20);
    }

    // set offsets
    this->setXAxisBias(gxbD / calibration_num_samples_);
    this->setYAxisBias(gybD / calibration_num_samples_);
    this->setZAxisBias(gzbD / calibration_num_samples_);

    return 1;
}

/* returns the gyro bias in the X direction, [rad/s] */
float Gyro::getXAxisBias() { return x_axis_bias_; }

/* returns the gyro bias in the Y direction, [rad/s] */
float Gyro::getYAxisBias() { return y_axis_bias_; }

/* returns the gyro bias in the Z direction, [rad/s] */
float Gyro::getZAxisBias() { return z_axis_bias_; }

/* sets the gyro bias in the X direction to bias, [rad/s] */
void Gyro::setXAxisBias(float bias) { x_axis_bias_ = bias; }

/* sets the gyro bias in the Y direction to bias, [rad/s] */
void Gyro::setYAxisBias(float bias) { y_axis_bias_ = bias; }

/* sets the gyro bias in the Z direction to bias, [rad/s] */
void Gyro::setZAxisBias(float bias) { z_axis_bias_ = bias; }

/* returns struct with the gyroscope readings in rad/s, with 2 decimals precision as 0.00 */
Sensor Gyro::read() {
    if (!x_axis_filter_.isAlphaDefined() || !y_axis_filter_.isAlphaDefined() || !z_axis_filter_.isAlphaDefined()) {
        std::cerr << "Falta definir la constante del filtro.\n";
    }

    Sensor return_data;

    // read Sensor data
    module_.readSensor();

    // return data with low pass filter
    float x_output = x_axis_filter_.filter(module_.getGyroX_rads());
    float y_output = y_axis_filter_.filter(module_.getGyroY_rads());
    float z_output = z_axis_filter_.filter(module_.getGyroZ_rads());

    // return data with offsets
    return_data.x = round(x_output - x_axis_bias_, 2);
    return_data.y = round(y_output - y_axis_bias_, 2);
    return_data.z = round(z_output - z_axis_bias_, 2);

    return return_data;
}

/* prints the gyro readings from Gyro::read() in a formatted output */
void Gyro::print(Sensor read_data) {   
    std::cout << "Accel (x,y,z): " << read_data.x << "kG_, " 
              << read_data.y << "kG_, " 
              << read_data.z << "kG_\n";
}

/* updates angles created from movement in the X and Y axis */
void Gyro::updateAngles() {
    // start gyro clock for angle measurement
    if (!gyro_timer_started_) clock_gettime(CLOCK_REALTIME, &gyro_prev_time_);

    gyro_timer_started_ = true;

    // Obtener tiempo transcurrido en segundos (dt)
    clock_gettime(CLOCK_REALTIME, &gyro_current_time);
    float dt = (gyro_current_time.tv_sec - gyro_prev_time_.tv_sec) +
                (gyro_current_time.tv_nsec - gyro_prev_time_.tv_nsec) / 1e9;

    // Actualizar ángulos usando la integración
    Sensor SensorData = read();
    x_axis_angle_ += SensorData.x * dt;
    y_axis_angle_ += SensorData.y * dt;

    // Guardar el tiempo actual como referencia para la siguiente iteración
    gyro_prev_time_ = gyro_current_time;
}

/* returns angle x axis created angle */
float Gyro::getXAxisAngle() { return x_axis_angle_; }

/* returns angle y axis created angle */
float Gyro::getYAxisAngle() { return y_axis_angle_; }

} // namespace pimu


// ===== Imu.hpp =====
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

