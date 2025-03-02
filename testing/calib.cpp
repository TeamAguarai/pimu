#include "Imu.h"

int main() {
    
    pimu::MPU9250 mpu;
    pimu::Imu imu(mpu);

    imu.begin();

    imu.calibrateAccel(30);
    
    std::cout << "Accel X Offset:";
    imu._accel.getXOffset().printEquation();
    
    std::cout << "\nAccel Y Offset:";
    imu._accel.getYOffset().printEquation();
    
    std::cout << "\nAccel Z Offset:";
    imu._accel.getZOffset().printEquation();

    while (true)
    {
        imu.print(imu.read());
        delay(1000);
    }
    


    
}