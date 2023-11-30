cmake_minimum_required(VERSION 3.13)

project(neuroPID)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")
add_executable(neuroPID WIN32 mlp.cpp nn_manager.cpp PID.cpp plant.cpp sample.cpp stdafx.cpp neuroPID_vc/main.cpp neuroPID.rc)

target_include_directories(neuroPID PUBLIC .)
