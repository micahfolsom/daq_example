# For compiling just the daq_example code, by itself, independent of
# pybind11, etc. This could also include the full Google Test suite,
# or whatever
cmake_minimum_required(VERSION 3.15...3.22)
project(daq_example VERSION "0.0.1")

# C++ options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/build_options.cmake)

# HDF5 - install with "sudo apt install libhdf5-dev" on Ubuntu
#find_package(HDF5 REQUIRED)
#include_directories(${HDF5_INCLUDE_DIRS})

# pybind11 - "pip install pybind11"
execute_process(
  COMMAND python -c
          "import pybind11; print(pybind11.get_cmake_dir())"
  OUTPUT_VARIABLE _tmp_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
message(STATUS ${CMAKE_PREFIX_PATH})
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS})

# DAQ source
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(DAQEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/daq.cpp)

add_library(daq SHARED ${DAQEX_SOURCES})
#target_link_libraries(daq PRIVATE pybind11::embed)
#target_link_libraries(daq PRIVATE ${HDF5_LIBRARIES})

# Simple test executables
add_executable(test_daq tests/test_daq.cpp ${DAQEX_SOURCES})
#target_link_libraries(test_daq PRIVATE daq)
target_link_libraries(test_daq PRIVATE pybind11::embed)
