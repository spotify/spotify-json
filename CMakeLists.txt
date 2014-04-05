cmake_minimum_required(VERSION 2.8.12)
project(json)

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

add_executable("json_test" main.cpp ${json_HEADERS})
