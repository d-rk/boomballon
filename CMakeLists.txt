
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

# settings for nano board
set(nano.build.mcu atmega328p)
set(nano.upload.speed 57600)

get_filename_component(SRC_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/src ABSOLUTE)

set(ARDUINO_BOARD  "micro"    CACHE STRING "board name")
set(ARDUINO_SERIAL_PORT  "com6"    CACHE STRING "port for serial communication" FORCE)
set(ARDUINO_UPLOAD_PORT  "com7"    CACHE STRING "port for uploading a firmware" FORCE)

message(STATUS "ARDUINO_BOARD: ${ARDUINO_BOARD}")
message(STATUS "ARDUINO_SERIAL_PORT: ${ARDUINO_SERIAL_PORT}")
message(STATUS "ARDUINO_UPLOAD_PORT: ${ARDUINO_UPLOAD_PORT}")

generate_arduino_firmware(BoomBallon
    SRCS ${SRC_FILES}
    HDRS ${HDR_FILES}
    INCLUDE_DIRS ${SRC_ABSOLUTE}
    PORT  ${ARDUINO_UPLOAD_PORT}
    BOARD ${ARDUINO_BOARD})

# the upload target can be used to upload the firmware to the arduino
# e.g. mingw32-make arduino-upload
add_custom_target(arduino-upload
    ${CMAKE_COMMAND} -E env BOARD=${ARDUINO_BOARD} SERIAL_PORT=${ARDUINO_SERIAL_PORT} UPLOAD_PORT=${ARDUINO_UPLOAD_PORT} cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arduino-upload.bat)

# the attach target can be used to attach to the serial port of a connected arduino
# e.g. mingw32-make attach
add_custom_target(attach
    ${CMAKE_COMMAND} -E env BOARD=${ARDUINO_BOARD} SERIAL_PORT=${ARDUINO_SERIAL_PORT} cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arduino-attach.bat)

