cmake_minimum_required(VERSION 3.10)
project(accel_attitude_estimator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(attitude_estimator
    src/main.cpp
    src/AttitudeEstimator.cpp
    src/LogParser.cpp
    src/ResultWriter.cpp
)

target_include_directories(attitude_estimator PRIVATE include)

if(MSVC)
    target_compile_options(attitude_estimator PRIVATE /W4 /permissive-)
else()
    target_compile_options(attitude_estimator PRIVATE -Wall -Wextra -Wpedantic)
endif()