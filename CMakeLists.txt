# Copyright (c) 2014-2016 Spotify AB
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

cmake_minimum_required(VERSION 3.2.0)
project(spotify-json)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(json_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

set(json_HEADERS
  include/spotify/json.hpp
  include/spotify/json/default_codec.hpp
  include/spotify/json/decode.hpp
  include/spotify/json/decode_exception.hpp
  include/spotify/json/decode_context.hpp
  include/spotify/json/encode.hpp
  include/spotify/json/encode_context.hpp
  include/spotify/json/encode_exception.hpp
  include/spotify/json/encoded_value.hpp
  include/spotify/json/json.hpp
  )

set(json_SOURCES
  )

set(json_codec_HEADERS
  include/spotify/json/codec/any_codec.hpp
  include/spotify/json/codec/any_value.hpp
  include/spotify/json/codec/array.hpp
  include/spotify/json/codec/boolean.hpp
  include/spotify/json/codec/boost.hpp
  include/spotify/json/codec/cast.hpp
  include/spotify/json/codec/chrono.hpp
  include/spotify/json/codec/codec_interface.hpp
  include/spotify/json/codec/empty_as.hpp
  include/spotify/json/codec/enumeration.hpp
  include/spotify/json/codec/eq.hpp
  include/spotify/json/codec/ignore.hpp
  include/spotify/json/codec/map.hpp
  include/spotify/json/codec/null.hpp
  include/spotify/json/codec/number.hpp
  include/spotify/json/codec/object.hpp
  include/spotify/json/codec/omit.hpp
  include/spotify/json/codec/one_of.hpp
  include/spotify/json/codec/smart_ptr.hpp
  include/spotify/json/codec/string.hpp
  include/spotify/json/codec/transform.hpp
  include/spotify/json/codec/tuple.hpp
  )

set(json_detail_HEADERS
  include/spotify/json/detail/bitset.hpp
  include/spotify/json/detail/cpuid.hpp
  include/spotify/json/detail/decode_helpers.hpp
  include/spotify/json/detail/encode_helpers.hpp
  include/spotify/json/detail/encode_integer.hpp
  include/spotify/json/detail/escape.hpp
  include/spotify/json/detail/macros.hpp
  include/spotify/json/detail/skip_chars.hpp
  include/spotify/json/detail/skip_value.hpp
  include/spotify/json/detail/stack.hpp
  )

set(json_detail_SOURCES
  src/detail/encode_integer.cpp
  src/detail/escape.cpp
  src/detail/escape_common.hpp
  src/detail/skip_chars.cpp
  src/detail/skip_chars_common.hpp
  src/detail/skip_value.cpp
  )

set(json_detail_SSE42_SOURCES
  src/detail/escape_sse42.cpp
  src/detail/skip_chars_sse42.cpp
  )

set(json_all_HEADERS
  ${json_HEADERS}
  ${json_codec_HEADERS}
  ${json_detail_HEADERS}
  )

set(json_all_SOURCES
  ${json_SOURCES}
  ${json_detail_SOURCES}
  ${json_detail_SSE42_SOURCES}
  )

source_group(spotify\\json         FILES ${json_HEADERS})
source_group(spotify\\json\\codec  FILES ${json_codec_HEADERS})
source_group(spotify\\json\\detail FILES ${json_detail_HEADERS})

set(json_library_TARGET "spotify-json")
add_library(${json_library_TARGET} STATIC ${json_all_HEADERS} ${json_all_SOURCES})
target_include_directories(${json_library_TARGET} PUBLIC ${json_INCLUDE_DIR})

option(SPOTIFY_JSON_USE_SSE42 "Build library with SSE 4.2 support (on x86 platforms)" ON)
if(SPOTIFY_JSON_USE_SSE42)
  if(WIN32)
    target_compile_definitions(${json_library_TARGET} PUBLIC SPOTIFY_JSON_USE_SSE42=1)
    target_compile_options(${json_library_TARGET} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
  else()
    target_compile_definitions(${json_library_TARGET} PUBLIC SPOTIFY_JSON_USE_SSE42=1)
    set_source_files_properties(${json_detail_SSE42_SOURCES} PROPERTIES COMPILE_FLAGS "-msse4.2")
  endif()
endif()

# Disable building double-conversion tests, since they fail on
# Windows due to the use of "/fp:fast" and bugs in the compiler.
# They also don't pass ASan at the moment.
set(BUILD_TESTING OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(vendor/double-conversion)

if(WIN32)
  target_compile_options(double-conversion PRIVATE "/MT$<$<CONFIG:Debug>:d>")
  target_compile_options(double-conversion PRIVATE "/w")
else()
  target_compile_options(double-conversion PRIVATE "-w")
endif()

set(double_conversion_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/vendor/double-conversion)

target_include_directories(${json_library_TARGET} PUBLIC ${double_conversion_INCLUDE_DIR})
target_link_libraries(${json_library_TARGET} double-conversion)

option(SPOTIFY_JSON_BUILD_TESTS "Build tests and benchmarks" ON)
if(SPOTIFY_JSON_BUILD_TESTS)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)

  find_package(Boost COMPONENTS chrono unit_test_framework system)

  if(Boost_FOUND)
    enable_testing()
    add_subdirectory(benchmark)
    add_subdirectory(test)
  else()
    message(STATUS "Specify BOOST_ROOT (and possibly BOOST_LIBRARYDIR) to build unit tests and benchmarks.")
  endif()
endif()
