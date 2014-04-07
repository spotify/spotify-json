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

cmake_minimum_required(VERSION 2.8.12)
project(json)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(json_detail_HEADERS
  detail/json_escape.hpp
  detail/json_locale.hpp
  detail/json_macros.hpp
  )

source_group(json\\detail FILES ${json_detail_HEADERS})

set(json_public_HEADERS
  json_buffer.hpp
  json_key.hpp
  json_pair.hpp
  json_writer.hpp
  )

source_group(json FILES ${json_public_HEADERS})

set(json_extension_HEADERS
  ext/json_boost.hpp
  ext/json_std.hpp
  )

source_group(json\\ext FILES ${json_extension_HEADERS})

set(json_HEADERS
  ${json_detail_HEADERS}
  ${json_public_HEADERS}
  ${json_extension_HEADERS}
  )

set(json_test_SOURCES
  test/test_json_key.cpp
  test/test_json_pair.cpp
  test/test_main.cpp
  )

source_group(json_test FILES ${json_test_SOURCES})

set(json_test_TARGET "json_test")

find_package(Boost REQUIRED)

if(Boost_FOUND)
  link_directories(${Boost_LIBRARY_DIRS})

  add_executable(${json_test_TARGET} ${json_test_SOURCES} ${json_HEADERS})
  target_include_directories(${json_test_TARGET} PUBLIC ${PROJECT_SOURCE_DIR})
  target_include_directories(${json_test_TARGET} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

  if(WIN32)
    target_compile_options(${json_test_TARGET} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
  endif(WIN32)

  if(NOT WIN32)
    target_link_libraries(${json_test_TARGET} boost_unit_test_framework)
  endif(NOT WIN32)

  enable_testing()

  include("AddBoostTests")
  ADD_BOOST_TESTS(${json_test_TARGET} ${json_test_SOURCES})
endif(Boost_FOUND)
