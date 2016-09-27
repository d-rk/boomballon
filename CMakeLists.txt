
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(BoomBallon C CXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-threadsafe-statics")

file(GLOB_RECURSE SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
string(REGEX REPLACE "[^;]+CMakeFiles/[^;]+;?" "" SRC_FILES "${SRC_FILES}") 

file(GLOB_RECURSE HDR_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
string(REGEX REPLACE "[^;]+CMakeFiles/[^;]+;?" "" HDR_FILES "${HDR_FILES}")

get_filename_component(SRC_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/src ABSOLUTE)

generate_arduino_firmware(BoomBallon
    SRCS ${SRC_FILES}
    HDRS ${HDR_FILES}
    INCLUDE_DIRS ${SRC_ABSOLUTE}
    PORT  com8
    BOARD micro)
