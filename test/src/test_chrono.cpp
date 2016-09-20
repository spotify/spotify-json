/*
 * Copyright (c) 2015 Spotify AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/chrono.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

BOOST_AUTO_TEST_CASE(json_codec_duration_should_construct) {
  duration<std::chrono::system_clock::duration>();
  duration<std::chrono::steady_clock::duration>();
  duration<std::chrono::high_resolution_clock::duration>();
}

BOOST_AUTO_TEST_CASE(json_codec_duration_should_construct_with_default_codec) {
  default_codec<std::chrono::system_clock::duration>();
  default_codec<std::chrono::steady_clock::duration>();
  default_codec<std::chrono::high_resolution_clock::duration>();
}

BOOST_AUTO_TEST_CASE(json_codec_duration_should_encode) {
  BOOST_CHECK_EQUAL(encode(std::chrono::system_clock::duration(5)), "5");
}

BOOST_AUTO_TEST_CASE(json_codec_duration_should_decode) {
  BOOST_CHECK(
      decode<std::chrono::system_clock::duration>("5") ==
      std::chrono::system_clock::duration(5));
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_construct) {
  time_point<std::chrono::system_clock::time_point>();
  time_point<std::chrono::steady_clock::time_point>();
  time_point<std::chrono::high_resolution_clock::time_point>();
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_construct_with_default_codec) {
  default_codec<std::chrono::system_clock::time_point>();
  default_codec<std::chrono::steady_clock::time_point>();
  default_codec<std::chrono::high_resolution_clock::time_point>();
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_encode) {
  using std::chrono::system_clock;
  BOOST_CHECK_EQUAL(encode(system_clock::time_point(system_clock::duration(5))), "5");
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_decode) {
  using std::chrono::system_clock;
  BOOST_CHECK(
      decode<system_clock::time_point>("5") ==
      system_clock::time_point(system_clock::duration(5)));
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
