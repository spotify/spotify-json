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

set(json_benchmark_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/benchmark/include)

set(json_benchmark_HEADERS
  include/spotify/json/benchmark/benchmark.hpp
  )

set(json_benchmark_SOURCES
  src/benchmark_main.cpp
  src/benchmark_number.cpp
  src/benchmark_object.cpp
  src/benchmark_string.cpp
  )

set(json_benchmark_TARGET "json_benchmark")

source_group(spotify\\json\\benchmark FILES ${json_benchmark_SOURCES} ${json_benchmark_HEADERS})

add_executable(${json_benchmark_TARGET} ${json_benchmark_SOURCES} ${json_benchmark_HEADERS})

target_include_directories(${json_benchmark_TARGET} PUBLIC ${json_benchmark_INCLUDE_DIR})
target_include_directories(${json_benchmark_TARGET} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(${json_benchmark_TARGET} ${json_library_TARGET})

if(WIN32)
  link_directories(${Boost_LIBRARY_DIRS})

  target_compile_options(${json_benchmark_TARGET} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
else()
  target_link_libraries(${json_benchmark_TARGET} ${Boost_LIBRARIES})
endif()

include("AddBoostTests")

ADD_BOOST_TESTS(${json_benchmark_TARGET} ${json_benchmark_SOURCES})
