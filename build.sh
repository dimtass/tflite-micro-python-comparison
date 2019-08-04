#!/bin/bash -e

# Usage examples: 
# $ ./build.sh src_cpp
# $ CLEANBUILD=true ./build.sh src_cpp
# $ CLEANBUILD=true USE_HAL_DRIVER=ON ./build.sh src_cpp_hal
# $ CLEANBUILD=true USE_HAL_DRIVER=ON USE_FREERTOS=ON ./build.sh src_cpp_freertos

echo "Building the project in Linux environment"

# Toolchain path
: ${TOOLCHAIN_DIR:=}
# select cmake toolchain
: ${CMAKE_TOOLCHAIN:=}
# select to clean previous builds
: ${CLEANBUILD:=false}
# select to create eclipse project files
: ${ECLIPSE_IDE:=false}
# Select source folder. Give a false one to trigger an error
: ${SRC:="src"}

# Set default arch to stm32
ARCHITECTURE=$(uname -i)
# default generator
IDE_GENERATOR="Unix Makefiles"
# Current working directory
WORKING_DIR=$(pwd)
# cmake scripts folder
SCRIPTS_CMAKE="${WORKING_DIR}/source/cmake"
# Compile objects in parallel, the -jN flag in make
PARALLEL=$(expr $(getconf _NPROCESSORS_ONLN) + 1)

if [ ! -d "source/${SRC}" ]; then
    echo -e "You need to specify the SRC parameter to point to the source code"
    exit 1
fi

if [ "${ECLIPSE}" == "true" ]; then
	IDE_GENERATOR="Eclipse CDT4 - Unix Makefiles" 
fi

BUILD_ARCH_DIR=${WORKING_DIR}/build-${ARCHITECTURE}

CMAKE_FLAGS="${CMAKE_FLAGS} \
                -DSRC=${SRC} \
                "

if [ "${CLEANBUILD}" == "true" ]; then
    echo "- removing build directory: ${BUILD_ARCH_DIR}"
    rm -rf ${BUILD_ARCH_DIR}
fi

echo "--- Pre-cmake ---"
echo "architecture      : ${ARCHITECTURE}"
echo "distclean         : ${CLEANBUILD}"
echo "parallel          : ${PARALLEL}"
echo "cmake flags       : ${CMAKE_FLAGS}"
echo "cmake scripts     : ${SCRIPTS_CMAKE}"
echo "IDE generator     : ${IDE_GENERATOR}"
echo "Threads           : ${PARALLEL}"

mkdir -p build-${ARCHITECTURE}
cd build-${ARCHITECTURE}

# setup cmake
cmake ../source -G"${IDE_GENERATOR}" ${CMAKE_FLAGS} -DARCHITECTURE=${ARCHITECTURE}

# build
make -j${PARALLEL} --no-print-directory
