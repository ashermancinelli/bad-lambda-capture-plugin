cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lambda-capture-checker CXX)

find_package(Clang REQUIRED)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
  )

include(AddLLVM)

configure_file(src/config.hpp.in ${PROJECT_BINARY_DIR}/config.hpp)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})

if(APPLE)
  add_definitions(-fno-rtti)
endif()

add_subdirectory(src)

# This file is valid C++ but will trigger error with the plugin
add_executable(dummy test/capture.cpp)
