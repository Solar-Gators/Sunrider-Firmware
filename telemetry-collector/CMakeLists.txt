cmake_minimum_required(VERSION 3.10)

# set the project name
project(TelemetryCollector)

add_subdirectory(src)

# add the executable
add_executable(Test src/main.cpp)

