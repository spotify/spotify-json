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
#include <spotify/json/error.hpp>

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
  try {
    auto ctx = decoding_context(string, string + strlen(string));
    const auto result = standard<std::string>().decode(ctx);
  } catch (const decode_exception &error) {
    return;
  }
  BOOST_CHECK(false);
}

/*
 * Decoding Simple Strings
 */

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_empty) {
  BOOST_CHECK_EQUAL(string_parse("\"\""), "");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_single_letter) {
  BOOST_CHECK_EQUAL(string_parse("\"a\""), "a");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_letters) {
  BOOST_CHECK_EQUAL(string_parse("\"abc\""), "abc");
}

/*
 * Decoding Invalid Strings
 */

BOOST_AUTO_TEST_CASE(json_codec_string_should_not_decode_invalid) {
  string_parse_fail("");
  string_parse_fail("\"");
}

/*
 * Decoding Escaped Strings
 */

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_escaped_characters) {
  BOOST_CHECK_EQUAL(string_parse("\"\\\"\""), "\"");
  BOOST_CHECK_EQUAL(string_parse("\"\\/\""), "/");
  BOOST_CHECK_EQUAL(string_parse("\"\\b\""), "\b");
  BOOST_CHECK_EQUAL(string_parse("\"\\f\""), "\f");
  BOOST_CHECK_EQUAL(string_parse("\"\\n\""), "\n");
  BOOST_CHECK_EQUAL(string_parse("\"\\r\""), "\r");
  BOOST_CHECK_EQUAL(string_parse("\"\\t\""), "\t");
  BOOST_CHECK_EQUAL(string_parse("\"\\\\\""), "\\");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_escaped_string_with_unescaped_parts) {
  BOOST_CHECK_EQUAL(string_parse("\"prefix\\nmiddle\\nsuffix\""), "prefix\nmiddle\nsuffix");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_decode_escaped_unicode) {
  // Examples from http://en.wikipedia.org/wiki/UTF-8#Examples
  BOOST_CHECK_EQUAL(string_parse("\"\\u0024\""), "\x24");
  BOOST_CHECK_EQUAL(string_parse("\"\\u00A2\""), "\xC2\xA2");
  BOOST_CHECK_EQUAL(string_parse("\"\\u20AC\""), "\xE2\x82\xAC");
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_not_decode_invalid_escaped_characters) {
  string_parse_fail("\"\\q\"");  // \q is not a valid escape sequence
}

BOOST_AUTO_TEST_CASE(json_codec_string_should_not_decode_invalid_unicode_escape_sequences) {
  string_parse_fail("\"\\u0\"");
  string_parse_fail("\"\\u01\"");
  string_parse_fail("\"\\u012\"");
  string_parse_fail("\"\\u_FFF\"");
  string_parse_fail("\"\\uF_FF\"");
  string_parse_fail("\"\\uFF_F\"");
  string_parse_fail("\"\\uFFF_\"");
}

/*
 * Encoding Simple Strings
 */

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
