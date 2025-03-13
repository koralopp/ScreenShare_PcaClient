cmake_minimum_required(VERSION 3.30)
project(ScreenShare_PcaClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(ScreenShare_PcaClient main.cpp)
