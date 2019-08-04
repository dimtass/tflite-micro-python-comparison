# Author: Dimitris Tassopoulos <dimtass@gmail.com>

cmake_minimum_required(VERSION 3.6)

set(PROJECT_NAME ${ARCHITECTURE}-mnist-tflite-micro)
project(${PROJECT_NAME} LANGUAGES C CXX)


# set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} ")
# find_library(ARM_MATH_LIB arm_cortexM7lfsp_math HINT ${CMAKE_SOURCE_DIR}/libs/CMSIS/DSP/Lib/GCC)
# message(STATUS "ARM_MATH_LIB: ${ARM_MATH_LIB}\n")

# If the toolchain path is not set in the build scsript then add it here${DEVICE_CONFIG_DIR}
# or in the toolchain cmake file
# set(TOOLCHAIN_DIR "/opt/toolchains/gcc-arm-none-eabi-8-2018-q4-major")

# set compiler optimisations
set(COMPILER_OPTIMISATION "-O3")

include (cmake/tensorflow_lite_micro.cmake)

set(CMAKE_BUILD_TYPE Release)

# Resolve the issue with Eclipse's indexer and C/C++ mixed source files
include(cmake/language_provider.cmake)
remove_from_SYSTEM_MACROS(CXX "__cplusplus;.*;")
create_language_provider("${CMAKE_BINARY_DIR}/.settings" "${GCC_PREFIX}-" "${CXX_STANDARD_FLAG}")


## Override toolchain flags ##
# CFLAGS
set (CPU_ARCH "")
set (CMAKE_C_FLAGS "${COMPILER_OPTIMISATION}\
                ${CPU_ARCH} \
                -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 \
                -Wall -lm  \
                -fno-unroll-loops -ffast-math -ftree-vectorize")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} " CACHE INTERNAL "c compiler flags")
# CXXFLAGS
set (CMAKE_CXX_FLAGS "${COMPILER_OPTIMISATION} \
            ${CPU_ARCH} \
            -fmessage-length=0 -ffunction-sections -fmessage-length=0 \
            -Wall -lm ")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -s")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE INTERNAL "cxx compiler flags")

# LDFLAGS
set (CMAKE_EXE_LINKER_FLAGS "${CPU_ARCH} -Wl,--gc-sections -lm")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" CACHE INTERNAL "executable linker flags")

message(STATUS "System Processor      : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS
    "BUILD FLAGS:\n"
    "   c flags         : ${CMAKE_C_FLAGS}\n"
    "   c++ flags       : ${CMAKE_CXX_FLAGS}\n"
    "   asm flags       : ${CMAKE_ASM_FLAGS}\n"
    "   ld flags        : ${CMAKE_EXE_LINKER_FLAGS}\n"
    "   optimizations   : ${COMPILER_OPTIMISATION}\n"
)


# add the source code directory
add_subdirectory(${SRC})
