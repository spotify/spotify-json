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

#include <spotify/json/codec/lenient.hpp>
#include <spotify/json/codec/string.hpp>
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

BOOST_AUTO_TEST_CASE(json_codec_lenient_should_construct) {
  lenient_t<string_t> codec(string());
}

BOOST_AUTO_TEST_CASE(json_codec_lenient_should_construct_with_helper) {
  lenient(string());
}

BOOST_AUTO_TEST_CASE(json_codec_lenient_should_encode) {
  const auto codec = lenient(string());
  BOOST_CHECK_EQUAL(encode(codec, "hello"), "\"hello\"");
}

BOOST_AUTO_TEST_CASE(json_codec_lenient_should_decode_successfully) {
  const auto codec = lenient(string());
  BOOST_CHECK_EQUAL(test_decode(codec, "\"hello\""), "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_lenient_should_decode_leniently) {
  const auto codec = lenient(string());
  BOOST_CHECK_EQUAL(test_decode(codec, "[{},true]"), "");
}

BOOST_AUTO_TEST_CASE(json_codec_lenient_should_fail_on_invalid_input) {
  const auto codec = lenient(string());
  test_decode_fail(codec, "a");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
