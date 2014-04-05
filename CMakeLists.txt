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

set(json_HEADERS
  ${json_detail_HEADERS}
  ${json_public_HEADERS}
  )

set(json_test_SOURCES
  test/test_json_key.cpp
  test/test_json_pair.cpp
  test/test_main.cpp
  )

source_group(json_test FILES ${json_test_SOURCES})

set(json_test_TARGET "json_test")

find_package(Boost REQUIRED unit_test_framework)

if(Boost_FOUND)
  link_directories(${Boost_LIBRARY_DIRS})

  add_executable(${json_test_TARGET} ${json_test_SOURCES} ${json_HEADERS})
  target_include_directories(${json_test_TARGET} PUBLIC ${PROJECT_SOURCE_DIR})
  target_include_directories(${json_test_TARGET} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

  if(NOT WIN32)
    target_link_libraries(${json_test_TARGET} ${Boost_LIBRARIES})
  endif(NOT WIN32)

  enable_testing()

  include("AddBoostTests")
  ADD_BOOST_TESTS(${json_test_TARGET} ${json_test_SOURCES})
endif(Boost_FOUND)
