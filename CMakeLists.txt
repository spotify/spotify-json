# Copyright (c) 2014 Spotify AB
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

cmake_minimum_required(VERSION 2.8.0)
project(spotify-json)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(json_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(json_HEADERS
  include/spotify/json/buffer.hpp
  include/spotify/json/codec.hpp
  include/spotify/json/error.hpp
  include/spotify/json/decoding_context.hpp
  include/spotify/json/encode_decode.hpp
  include/spotify/json/error.hpp
  include/spotify/json/key.hpp
  include/spotify/json/pair.hpp
  include/spotify/json/standard.hpp
  include/spotify/json/writer.hpp
  )

set(json_codec_HEADERS
  include/spotify/json/codec/any.hpp
  include/spotify/json/codec/array.hpp
  include/spotify/json/codec/boolean.hpp
  include/spotify/json/codec/cast.hpp
  include/spotify/json/codec/codec_interface.hpp
  include/spotify/json/codec/equals.hpp
  include/spotify/json/codec/integer.hpp
  include/spotify/json/codec/lenient.hpp
  include/spotify/json/codec/map.hpp
  include/spotify/json/codec/null.hpp
  include/spotify/json/codec/object.hpp
  include/spotify/json/codec/one_of.hpp
  include/spotify/json/codec/real.hpp
  include/spotify/json/codec/smart_ptr.hpp
  include/spotify/json/codec/string.hpp
  )

set(json_detail_HEADERS
  include/spotify/json/detail/char_traits.hpp
  include/spotify/json/detail/decoding_helpers.hpp
  include/spotify/json/detail/escape.hpp
  include/spotify/json/detail/iterator.hpp
  include/spotify/json/detail/macros.hpp
  include/spotify/json/detail/primitive_encoder.hpp
  )

set(json_extension_HEADERS
  include/spotify/json/extension/boost.hpp
  include/spotify/json/extension/std.hpp
  )

set(json_all_HEADERS
  ${json_HEADERS}
  ${json_codec_HEADERS}
  ${json_detail_HEADERS}
  ${json_extension_HEADERS}
  )

set(json_test_SOURCES
  test/test_any.cpp
  test/test_array.cpp
  test/test_boolean.cpp
  test/test_buffer.cpp
  test/test_cast.cpp
  test/test_char_traits.cpp
  test/test_codec_interface.cpp
  test/test_decoding_context.cpp
  test/test_decoding_helpers.cpp
  test/test_escape.cpp
  test/test_equals.cpp
  test/test_key.cpp
  test/test_lenient.cpp
  test/test_main.cpp
  test/test_map.cpp
  test/test_null.cpp
  test/test_object.cpp
  test/test_one_of.cpp
  test/test_pair.cpp
  test/test_primitive_encoder.cpp
  test/test_real.cpp
  test/test_smart_ptr.cpp
  test/test_string.cpp
  test/test_writer.cpp
  )

set(json_test_TARGET "json_test")

source_group(spotify\\json            FILES ${json_HEADERS})
source_group(spotify\\json\\codec     FILES ${json_codec_HEADERS})
source_group(spotify\\json\\detail    FILES ${json_detail_HEADERS})
source_group(spotify\\json\\extension FILES ${json_extension_HEADERS})
source_group(spotify\\json\\test      FILES ${json_test_SOURCES})

if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

if(Boost_FOUND)
  link_directories(${Boost_LIBRARY_DIRS})

  add_executable(${json_test_TARGET} ${json_test_SOURCES} ${json_all_HEADERS})
  target_include_directories(${json_test_TARGET} PUBLIC ${json_INCLUDE_DIR})
  target_include_directories(${json_test_TARGET} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

  if(WIN32)
    target_compile_options(${json_test_TARGET} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
  else()
    target_compile_options(${json_test_TARGET} PRIVATE "-fvisibility=hidden")
    target_compile_options(${json_test_TARGET} PRIVATE "-std=c++11")
    target_link_libraries(${json_test_TARGET} boost_unit_test_framework)
  endif()

  add_subdirectory(vendor/double-conversion)

  if(WIN32)
    target_compile_options(double-conversion PRIVATE "/MT$<$<CONFIG:Debug>:d>")
  endif()

  target_include_directories(${json_test_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/double-conversion)
  target_link_libraries(${json_test_TARGET} double-conversion)

  enable_testing()

  include("AddBoostTests")
  ADD_BOOST_TESTS(${json_test_TARGET} ${json_test_SOURCES})
endif(Boost_FOUND)
