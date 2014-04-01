cmake_minimum_required(VERSION 2.8.5)
project(json)

set(json_SOURCES
  json_buffer.hpp
  json_escape.hpp
  json_key.hpp
  json_locale.hpp
  json_pair.hpp
  json_writer.hpp
  main.cpp
  )

add_executable(json_test ${json_SOURCES})
