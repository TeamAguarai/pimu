#include "Imu.h"

int main() {
    pimu::Imu imu;
    imu.begin();
    int c = 0;
    imu.setGyroFilters(0.85);
    while (true)
    {
        c++;
        std::cout << "Lectura[" << c << "]\n";

        std::cout << "X Angle: " << imu.getXAngle() << "\n"; 
        std::cout << "Y Angle: " << imu.getYAngle() << "\n";
        
        delay(100);
    }

    
}