spotify-json
============

Fast and nice to use C++ JSON writer library (header-only).

Building and running tests
--------------------------
### Requirements
  * CMake ([http://www.cmake.org](http://www.cmake.org/))
  * Boost ([http://www.boost.org](http://www.boost.org/))

### 1. Make CMake find Boost
    export BOOST_ROOT=/path/to/boost
    export BOOST_LIBRARYDIR=/path/to/boost/lib/

### 2. Run CMake
    mkdir build
    cd build
    cmake -G <generator-name> ..

Run "cmake --help" for a list of generators available on your system.

### 3. Build project with Visual Studio / Xcode / Ninja

### 4. Run CTest
    cd build
    ctest -j 8
