/*
 * Copyright (c) 2015-2016 Spotify AB
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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/array.hpp>
#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/omit.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename Parsed = std::vector<bool>>
Parsed array_parse(const char *not_array) {
  const auto codec = default_codec<Parsed>();
  auto ctx = decode_context(not_array, not_array + strlen(not_array));
  const auto result = codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  return result;
}

template <typename Parsed = std::vector<bool>>
void array_parse_should_fail(const char *not_array) {
  const auto codec = default_codec<Parsed>();
  auto ctx = decode_context(not_array, not_array + strlen(not_array));
  BOOST_CHECK_THROW(codec.decode(ctx), decode_exception);
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_with_helper) {
  array<std::vector<bool>>(boolean());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_vector_with_default_codec) {
  default_codec<std::vector<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_array_with_default_codec) {
  default_codec<std::array<bool, 3>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_deque_with_default_codec) {
  default_codec<std::deque<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_set_with_default_codec) {
  default_codec<std::set<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_unordered_set_with_default_codec) {
  default_codec<std::unordered_set<bool>>();
}

/*
 * Vector Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_vector) {
  BOOST_CHECK(array_parse("[]").empty());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_single_element_vector) {
  BOOST_CHECK(array_parse("[true]") == std::vector<bool>({ true }));
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_two_elements_vector) {
  BOOST_CHECK(array_parse("[true,false]") == std::vector<bool>({ true, false }));
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_not_decode_otherwise_vector) {
  array_parse_should_fail("");
  array_parse_should_fail("[");
  array_parse_should_fail("[[");
  array_parse_should_fail("[false");
  array_parse_should_fail("[false,true,]");
  array_parse_should_fail("[false,true,");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_accept_inner_codec_vector) {
  const auto inner_codec(boolean());
  const auto array_codec(array<std::vector<bool>>(inner_codec));
  BOOST_CHECK(decode(array_codec, "[]").empty());
}

/*
 * Array Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_array) {
  const auto res = array_parse<std::array<int, 0>>("[]");
  BOOST_CHECK(res.empty());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_single_element_array) {
  const auto res = array_parse<std::array<bool, 1>>("[true]");
  const auto expected = std::array<bool, 1>{{ true }};
  BOOST_CHECK(res == expected);
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_two_elements_array) {
  const auto res = array_parse<std::array<bool, 2>>("[true,false]");
  const auto expected = std::array<bool, 2>{{ true, false }};
  BOOST_CHECK(res == expected);
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_not_decode_otherwise_array) {
  array_parse_should_fail<std::array<bool, 1>>("[]");
  array_parse_should_fail<std::array<bool, 0>>("[true]");

  array_parse_should_fail<std::array<bool, 0>>("");
  array_parse_should_fail<std::array<bool, 1>>("");
  array_parse_should_fail<std::array<bool, 0>>("[");
  array_parse_should_fail<std::array<bool, 1>>("[");
  array_parse_should_fail<std::array<bool, 0>>("[[");
  array_parse_should_fail<std::array<bool, 1>>("[[");
  array_parse_should_fail<std::array<bool, 0>>("[false");
  array_parse_should_fail<std::array<bool, 1>>("[false");
  array_parse_should_fail<std::array<bool, 0>>("[false,true,]");
  array_parse_should_fail<std::array<bool, 1>>("[false,true,]");
  array_parse_should_fail<std::array<bool, 0>>("[false,true,");
  array_parse_should_fail<std::array<bool, 1>>("[false,true,");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_accept_inner_codec_array) {
  const auto inner_codec(boolean());
  const auto array_codec(array<std::vector<bool>>(inner_codec));
  BOOST_CHECK(decode(array_codec, "[]").empty());
}

/*
 * Vector Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_empty_vector) {
  const std::vector<bool> vec;
  BOOST_CHECK_EQUAL(encode(vec), "[]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_single_element_vector) {
  const std::vector<bool> vec = { true };
  BOOST_CHECK_EQUAL(encode(vec), "[true]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_two_elements_vector) {
  const std::vector<bool> vec = { false, true };
  BOOST_CHECK_EQUAL(encode(vec), "[false,true]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_not_encode_omitted_elements_vector) {
  const std::vector<bool> vec = { false, true };
  const auto codec = array<std::vector<bool>>(omit<bool>());
  BOOST_CHECK_EQUAL(encode(codec, vec), "[]");
}

/*
 * Array Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_empty_array) {
  const std::array<int, 0> arr = {};
  BOOST_CHECK_EQUAL(encode(arr), "[]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_single_element_array) {
  const std::array<int, 1> arr{{ 7 }};
  BOOST_CHECK_EQUAL(encode(arr), "[7]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_two_elements_array) {
  const std::array<int, 2> arr{{ 4, 9 }};
  BOOST_CHECK_EQUAL(encode(arr), "[4,9]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_not_encode_omitted_elements_array) {
  const std::array<bool, 2> arr{{ false, true }};
  const auto codec = array<std::array<bool, 2>>(omit<bool>());
  BOOST_CHECK_EQUAL(encode(codec, arr), "[]");
}

/*
 * Set Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_set) {
  BOOST_CHECK(array_parse<std::set<bool>>("[]").empty());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_unordered_set) {
  BOOST_CHECK(array_parse<std::unordered_set<bool>>("[]").empty());
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
