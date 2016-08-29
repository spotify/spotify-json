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

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/string.hpp>
#include <spotify/json/codec/eq.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename Codec>
typename Codec::object_type test_decode(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK_EQUAL(c.position, c.end);
  return obj;
}

template <typename Codec>
void test_decode_fail(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  BOOST_CHECK_THROW(codec.decode(c), decode_exception);
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_eq_should_construct) {
  eq_t<string_t> codec(string(), "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_eq_should_construct_with_helper_with_codec) {
  eq(string(), "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_eq_should_construct_with_helper) {
  eq(std::string("hello"));
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_eq_should_decode) {
  const auto codec = eq(std::string("A"));
  BOOST_CHECK_EQUAL(test_decode(codec, "\"A\""), "A");
}

BOOST_AUTO_TEST_CASE(json_codec_eq_should_enforce_correct_value_with_decode) {
  const auto codec = eq(std::string("A"));
  test_decode_fail(codec, "\"B\"");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_eq_should_encode_original_value) {
  const auto codec = eq(std::string("A"));
  BOOST_CHECK_EQUAL(encode(codec, "B"), "\"A\"");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
