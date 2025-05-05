cmake_minimum_required(VERSION 3.4)
project(pimu)

add_subdirectory(python-lib/pybind11)

pybind11_add_module(pimu py-library-bindings.cpp)

# target_link_libraries(pimu PRIVATE wiringPi)
target_link_libraries(pimu PRIVATE)
