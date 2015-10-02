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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/array.hpp>
#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template<typename Parsed = std::vector<bool>>
Parsed array_parse(const char *not_array) {
  const auto codec = default_codec<Parsed>();
  auto ctx = decoding_context(not_array, not_array + strlen(not_array));
  const auto result = codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  return result;
}

void array_parse_should_fail(const char *not_array) {
  const auto codec = default_codec<std::vector<bool>>();
  auto ctx = decoding_context(not_array, not_array + strlen(not_array));
  BOOST_CHECK_THROW(codec.decode(ctx), decode_exception);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_array) {
  BOOST_CHECK(array_parse("[]").empty());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_single_element) {
  BOOST_CHECK(array_parse("[true]") == std::vector<bool>({ true }));
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_two_elements) {
  BOOST_CHECK(array_parse("[true,false]") == std::vector<bool>({ true, false }));
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_not_decode_otherwise) {
  array_parse_should_fail("");
  array_parse_should_fail("[");
  array_parse_should_fail("[[");
  array_parse_should_fail("[false");
  array_parse_should_fail("[false,true,]");
  array_parse_should_fail("[false,true,");
}


BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_empty) {
  std::vector<bool> vec;
  BOOST_CHECK_EQUAL(encode(vec), "[]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_single_element) {
  const std::vector<bool> vec = { true };
  BOOST_CHECK_EQUAL(encode(vec), "[true]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_encode_two_elements) {
  const std::vector<bool> vec = { false, true };
  BOOST_CHECK_EQUAL(encode(vec), "[false,true]");
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_with_helper) {
  array<std::vector<bool>>(boolean());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_vector_with_default_codec) {
  default_codec<std::vector<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_deque_with_default_codec) {
  default_codec<std::deque<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_set_with_default_codec) {
  default_codec<std::set<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_set) {
  BOOST_CHECK(array_parse<std::set<bool>>("[]").empty());
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_construct_unordered_set_with_default_codec) {
  default_codec<std::unordered_set<bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_array_should_decode_empty_unordered_set) {
  BOOST_CHECK(array_parse<std::unordered_set<bool>>("[]").empty());
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
