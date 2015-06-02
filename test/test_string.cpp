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

#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/map.hpp>
#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

std::string string_parse(const char *string) {
  const auto codec = standard<std::string>();
  auto ctx = decoding_context(string, string + strlen(string));

  const auto result = codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  BOOST_CHECK(!ctx.has_failed());

  return result;
}

void string_parse_fail(const char *string) {
  auto ctx = decoding_context(string, string + strlen(string));
  const auto result = standard<std::string>().decode(ctx);
  BOOST_CHECK(ctx.has_failed());
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_empty) {
  BOOST_CHECK_EQUAL(string_parse("\"\""), "");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_single_letter) {
  BOOST_CHECK_EQUAL(string_parse("\"a\""), "a");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_letters) {
  BOOST_CHECK_EQUAL(string_parse("\"abc\""), "abc");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_not_decode_invalid) {
  string_parse_fail("");
  string_parse_fail("\"");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_encode_empty) {
  BOOST_CHECK_EQUAL(encode(std::string()), "\"\"");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_encode_single_character) {
  BOOST_CHECK_EQUAL(encode(std::string("a")), "\"a\"");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_encode_escaped_character) {
  BOOST_CHECK_EQUAL(encode(std::string("\"")), "\"\\\"\"");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_construct_with_helper) {
  string();
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_construct_with_standard) {
  standard<std::string>();
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
