/* 
    This file only works with ./build_python_library.py 
    #include files are generated with ./build_python_library.py
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "python-lib/pimu.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pimu, m) {
    m.doc() = "Bindings for pampas library (namespace pimu)";

    // Enums for MPU9250
    py::enum_<pimu::MPU9250::GyroRange>(m, "GyroRange")
        .value("GYRO_RANGE_250DPS", pimu::MPU9250::GyroRange::GYRO_RANGE_250DPS)
        .value("GYRO_RANGE_500DPS", pimu::MPU9250::GyroRange::GYRO_RANGE_500DPS)
        .value("GYRO_RANGE_1000DPS", pimu::MPU9250::GyroRange::GYRO_RANGE_1000DPS)
        .value("GYRO_RANGE_2000DPS", pimu::MPU9250::GyroRange::GYRO_RANGE_2000DPS)
        .export_values();

    py::enum_<pimu::MPU9250::AccelRange>(m, "AccelRange")
        .value("ACCEL_RANGE_2G", pimu::MPU9250::AccelRange::ACCEL_RANGE_2G)
        .value("ACCEL_RANGE_4G", pimu::MPU9250::AccelRange::ACCEL_RANGE_4G)
        .value("ACCEL_RANGE_8G", pimu::MPU9250::AccelRange::ACCEL_RANGE_8G)
        .value("ACCEL_RANGE_16G", pimu::MPU9250::AccelRange::ACCEL_RANGE_16G)
        .export_values();

    py::enum_<pimu::MPU9250::DlpfBandwidth>(m, "DlpfBandwidth")
        .value("DLPF_BANDWIDTH_184HZ", pimu::MPU9250::DlpfBandwidth::DLPF_BANDWIDTH_184HZ)
        .value("DLPF_BANDWIDTH_92HZ", pimu::MPU9250::DlpfBandwidth::DLPF_BANDWIDTH_92HZ)
        .value("DLPF_BANDWIDTH_41HZ", pimu::MPU9250::DlpfBandwidth::DLPF_BANDWIDTH_41HZ)
        .value("DLPF_BANDWIDTH_20HZ", pimu::MPU9250::DlpfBandwidth::DLPF_BANDWIDTH_20HZ)
        .value("DLPF_BANDWIDTH_10HZ", pimu::MPU9250::DlpfBandwidth::DLPF_BANDWIDTH_10HZ)
        .value("DLPF_BANDWIDTH_5HZ",  pimu::MPU9250::DlpfBandwidth::DLPF_BANDWIDTH_5HZ)
        .export_values();

    py::enum_<pimu::MPU9250::LpAccelOdr>(m, "LpAccelOdr")
        .value("LP_ACCEL_ODR_0_24HZ",  pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_0_24HZ)
        .value("LP_ACCEL_ODR_0_49HZ",  pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_0_49HZ)
        .value("LP_ACCEL_ODR_0_98HZ",  pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_0_98HZ)
        .value("LP_ACCEL_ODR_1_95HZ",  pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_1_95HZ)
        .value("LP_ACCEL_ODR_3_91HZ",  pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_3_91HZ)
        .value("LP_ACCEL_ODR_7_81HZ",  pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_7_81HZ)
        .value("LP_ACCEL_ODR_15_63HZ", pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_15_63HZ)
        .value("LP_ACCEL_ODR_31_25HZ", pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_31_25HZ)
        .value("LP_ACCEL_ODR_62_50HZ", pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_62_50HZ)
        .value("LP_ACCEL_ODR_125HZ",   pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_125HZ)
        .value("LP_ACCEL_ODR_250HZ",   pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_250HZ)
        .value("LP_ACCEL_ODR_500HZ",   pimu::MPU9250::LpAccelOdr::LP_ACCEL_ODR_500HZ)
        .export_values();

    // Class MPU9250
    py::class_<pimu::MPU9250>(m, "MPU9250")
        .def(py::init<>())
        .def("begin", &pimu::MPU9250::begin)
        .def("writeRegister", &pimu::MPU9250::writeRegister)
        .def("readRegisters", &pimu::MPU9250::readRegisters)
        .def("setAccelRange", &pimu::MPU9250::setAccelRange)
        .def("setGyroRange", &pimu::MPU9250::setGyroRange)
        .def("setDlpfBandwidth", &pimu::MPU9250::setDlpfBandwidth)
        .def("setSrd", &pimu::MPU9250::setSrd)
        .def("enableDataReadyInterrupt", &pimu::MPU9250::enableDataReadyInterrupt)
        .def("disableDataReadyInterrupt", &pimu::MPU9250::disableDataReadyInterrupt)
        .def("enableWakeOnMotion", &pimu::MPU9250::enableWakeOnMotion)
        .def("readSensor", &pimu::MPU9250::readSensor)
        .def("getGyroX_rads", &pimu::MPU9250::getGyroX_rads)
        .def("getGyroY_rads", &pimu::MPU9250::getGyroY_rads)
        .def("getGyroZ_rads", &pimu::MPU9250::getGyroZ_rads)
        .def("getAccelX_mss", &pimu::MPU9250::getAccelX_mss)
        .def("getAccelY_mss", &pimu::MPU9250::getAccelY_mss)
        .def("getAccelZ_mss", &pimu::MPU9250::getAccelZ_mss)
        .def("getMagX_uT", &pimu::MPU9250::getMagX_uT)
        .def("getMagY_uT", &pimu::MPU9250::getMagY_uT)
        .def("getMagZ_uT", &pimu::MPU9250::getMagZ_uT)
        .def("getTemperature_C", &pimu::MPU9250::getTemperature_C);

    // Class Imu
    py::class_<pimu::Imu>(m, "Imu")
        .def(py::init<pimu::MPU9250&>())
        .def("begin", &pimu::Imu::begin)
        .def("calibrateGyro", &pimu::Imu::calibrateGyro)
        .def("calibrateAccel", &pimu::Imu::calibrateAccel)
        .def("read", &pimu::Imu::read)
        .def("print", &pimu::Imu::print)
        .def("setGyroFilters", &pimu::Imu::setGyroFilters)
        .def("startUpdateThread", &pimu::Imu::startUpdateThread)
        .def("getXAxisAngle", &pimu::Imu::getXAxisAngle)
        .def("getYAxisAngle", &pimu::Imu::getYAxisAngle);
}
