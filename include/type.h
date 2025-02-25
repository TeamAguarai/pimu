namespace pimu
{

/* three axis sensor return data (gyro, accel or mag) */
struct sensor
{
    /* sensor axis */
    float x, y, z;
};

/* mpu module return data (gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z) */
struct mpu_module 
{
    /* gyro axis */
    float gx, gy, gz; 
    /* accel axis */
    float ax, ay, az;
};

struct calibration {
    float gyro_xbias, gyro_ybias, gyro_zbias;
};

}// namespace pimu


