cmake_minimum_required(VERSION 3.8)
project(nvidia-oc CXX)

set(CMAKE_CXX_STANDARD 11)

add_executable(nvidia-oc nvidia-oc.cpp)
target_link_libraries(nvidia-oc X11 XNVCtrl)
