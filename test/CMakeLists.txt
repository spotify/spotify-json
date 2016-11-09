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

set(json_test_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)

set(json_test_HEADERS
  include/spotify/json/test/only_true.hpp
  )

set(json_test_SOURCES
  src/test_any_codec.cpp
  src/test_any_value.cpp
  src/test_array.cpp
  src/test_bitset.cpp
  src/test_boolean.cpp
  src/test_boost.cpp
  src/test_cast.cpp
  src/test_chrono.cpp
  src/test_codec_interface.cpp
  src/test_decode.cpp
  src/test_decode_context.cpp
  src/test_decode_helpers.cpp
  src/test_empty_as.cpp
  src/test_encode.cpp
  src/test_encode_context.cpp
  src/test_encode_helpers.cpp
  src/test_encode_integer.cpp
  src/test_encoded_value.cpp
  src/test_enumeration.cpp
  src/test_eq.cpp
  src/test_escape.cpp
  src/test_ignore.cpp
  src/test_macros.cpp
  src/test_main.cpp
  src/test_map.cpp
  src/test_null.cpp
  src/test_number.cpp
  src/test_object.cpp
  src/test_omit.cpp
  src/test_one_of.cpp
  src/test_skip_chars.cpp
  src/test_skip_value.cpp
  src/test_smart_ptr.cpp
  src/test_stack.cpp
  src/test_string.cpp
  src/test_transform.cpp
  src/test_tuple.cpp
  src/test_umbrella.cpp
  )

set(json_test_TARGET "json_test")

source_group(spotify\\json\\test FILES ${json_test_SOURCES})
source_group(spotify\\json\\test FILES ${json_test_HEADERS})

add_executable(${json_test_TARGET} ${json_test_SOURCES} ${json_test_HEADERS})

set_property(TARGET ${json_test_TARGET} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${json_test_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

if(WIN32)
  target_compile_options(${json_test_TARGET} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
endif()

target_include_directories(${json_test_TARGET} PUBLIC ${json_test_INCLUDE_DIR})
target_include_directories(${json_test_TARGET} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(${json_test_TARGET} ${json_library_TARGET})
target_link_libraries(${json_test_TARGET} ${Boost_LIBRARIES})

add_test(${json_test_TARGET} ${json_test_TARGET})
