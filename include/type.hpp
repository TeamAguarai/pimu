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