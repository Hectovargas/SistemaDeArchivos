cmake_minimum_required(VERSION 3.10)

project(blockdevice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(blockdevice BlockDevice.hpp filesystem.hpp filesystem.cpp Interfaz.cpp)
