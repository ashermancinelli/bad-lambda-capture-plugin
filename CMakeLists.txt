#[[#############################################################################

Lambda Checker

Clang plugin to check lambda capture clauses to ensure pointer-like member
variables are not captured by a lambda in an instance method via the `this`
pointer.

This plugin depends on LLVM and Clang version 10.0.1 through 12.0.0. This may
work with other versions of Clang and LLVM, however this has not been tested.

See README.md for more information.

]]##############################################################################

cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(LambdaChecker CXX)

set(CHECKER_VERSION "0.0.1")

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

if(NOT LLVM_ENABLE_RTTI)
  add_definitions(-fno-rtti)
endif()

add_subdirectory(src)

foreach(target ${PROJECT_NAME} SyntaxChecker)
  target_link_libraries(${PROJECT_NAME} PUBLIC 
    "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
  target_link_libraries(${PROJECT_NAME}Standalone PUBLIC 
    "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
endforeach()

# This file is valid C++ but will trigger error with the lambda plugin. Used
# as an example for users to test the plugin.
add_executable(capture-dummy test/capture.cpp)

# Similar to above, but for various syntax issues
add_executable(syntax-dummy test/syntax.cpp)
