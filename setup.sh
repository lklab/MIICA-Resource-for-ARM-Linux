#!/bin/bash

### dependencies ###
#   tool : wget, unzip, tar, cmake, make
#   project : TOOLCHAIN_REPO, SOEM_REPO
####################

RELEASE="release/"

TOOLCHAIN_NAME="gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf"
TOOLCHAIN_COMP_NAME=${TOOLCHAIN_NAME}".tar.xz"
TOOLCHAIN_ROOT=${RELEASE}${TOOLCHAIN_NAME}
TOOLCHAIN_COMP_PATH=${TOOLCHAIN_ROOT}".tar.xz"
TOOLCHAIN_REPO="https://releases.linaro.org/components/toolchain/binaries/7.2-2017.11/arm-linux-gnueabihf/"${TOOLCHAIN_COMP_NAME}

CROSS_COMPILER_NAME="/bin/arm-linux-gnueabihf-"
CROSS_COMPILER_ROOT=$(pwd)/${TOOLCHAIN_ROOT}${CROSS_COMPILER_NAME}
CMAKE_TOOLCHAIN="toolchain.cmake"
CMAKE_TOOLCHAIN_TEMPLATE=${CMAKE_TOOLCHAIN}".template"

SOEM_COMP="master.zip"
SOEM_REPO="https://github.com/OpenEtherCATsociety/SOEM/archive/"${SOEM_COMP}
SOEM_ROOT="SOEM-master"

# create release directory
if [ ! -d "${RELEASE}" ] ; then
	mkdir ${RELEASE}
fi

# download toolchain
if [ ! -d "${TOOLCHAIN_ROOT}" ] ; then
	if [ ! -f "${TOOLCHAIN_COMP_NAME}" ] ; then
		wget ${TOOLCHAIN_REPO}
	fi
	tar xf ${TOOLCHAIN_COMP_NAME} -C ${RELEASE}
fi

# create cmake toolchain configuration file for release
SED_COMMAND="s/root_path/%(path)s"$(echo ${TOOLCHAIN_NAME}${CROSS_COMPILER_NAME} | sed "s/\//\\\\\//g")"/"
if [ ! -f "${RELEASE}${CMAKE_TOOLCHAIN}" ] ; then
	cat ${CMAKE_TOOLCHAIN_TEMPLATE} | sed ${SED_COMMAND} > ${RELEASE}${CMAKE_TOOLCHAIN}
fi

# create cmake toolchain configuration file for library build
SED_COMMAND="s/root_path/"$(echo ${CROSS_COMPILER_ROOT} | sed "s/\//\\\\\//g")"/"
if [ ! -f "${CMAKE_TOOLCHAIN}" ] ; then
	cat ${CMAKE_TOOLCHAIN_TEMPLATE} | sed ${SED_COMMAND} > ${CMAKE_TOOLCHAIN}
fi

# download SOEM
if [ ! -d "${SOEM_ROOT}" ] ; then
	if [ ! -f "${SOEM_COMP}" ] ; then
		wget ${SOEM_REPO}
	fi
	unzip ${SOEM_COMP}
fi

# build SOEM
if [ ! -d build_SOEM ] ; then
	mkdir build_SOEM
fi
cd build_SOEM
cmake -DCMAKE_TOOLCHAIN_FILE=../${CMAKE_TOOLCHAIN} ../${SOEM_ROOT}
make
make install
cd ..

# build resource libraries
if [ ! -d build_lib ] ; then
	mkdir build_lib
fi
cd build_lib
cmake -DCMAKE_TOOLCHAIN_FILE=../${CMAKE_TOOLCHAIN} ..
make
cd ..

# copy result files to release
cp options.txt ${RELEASE}
if [ ! -d ${RELEASE}/lib ] ; then
	mkdir ${RELEASE}/lib
fi
cp build_lib/lib*.a ${RELEASE}/lib
cp SOEM-master/install/lib/libsoem.a ${RELEASE}/lib

# setup finished
echo "setup finished"
