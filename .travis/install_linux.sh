#!/bin/bash

DEPS_DIR="${TRAVIS_BUILD_DIR}/deps"
mkdir -p ${DEPS_DIR} && cd ${DEPS_DIR}

function install_boost {
  BOOST_LIBRARIES="chrono,system,test"
  BOOST_VERSION="1.62.0"
  BOOST_URL="https://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/boost_${BOOST_VERSION//\./_}.tar.gz"
  BOOST_DIR="${DEPS_DIR}/boost"
  echo "Downloading Boost ${BOOST_VERSION} from ${BOOST_URL}"
  mkdir -p ${BOOST_DIR} && cd ${BOOST_DIR}
  wget -O - ${BOOST_URL} | tar --strip-components=1 -xz -C ${BOOST_DIR} || exit 1
  ./bootstrap.sh --with-libraries=${BOOST_LIBRARIES} && ./b2
  export BOOST_ROOT=${BOOST_DIR}
}

function install_cmake {
  CMAKE_VERSION="3.6.2"
  CMAKE_URL="https://cmake.org/files/v3.6/cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz"
  CMAKE_DIR="${DEPS_DIR}/cmake"
  CMAKE_BIN="${CMAKE_DIR}/bin"
  echo "Downloading CMake ${CMAKE_VERSION} from ${CMAKE_URL}"
  mkdir -p ${CMAKE_DIR}
  wget --no-check-certificate -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C ${CMAKE_DIR} || exit 1
  export PATH=${CMAKE_BIN}:${PATH}
}

install_boost # at least version 1.60
install_cmake # at least version 3.2
echo "Installed build dependecies."
echo "  - Boost: ${BOOST_ROOT}"
echo "  - CMake: ${CMAKE_BIN}"
