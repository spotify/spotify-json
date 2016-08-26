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
#include <spotify/json/codec/enumeration.hpp>
#include <spotify/json/encode_decode.hpp>
#include <spotify/json/encode_exception.hpp>

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

enum class Test {
  A,
  B
};

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_construct) {
  enumeration_t<Test, string_t> codec(
      string(),
      std::vector<std::pair<Test, std::string>>());
}

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_construct_with_helper_with_codec) {
  enumeration<Test>(string(), { { Test::A, "A" } });
}

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_construct_with_multiple_parameters_helper_with_codec) {
  enumeration<Test>(string(), { { Test::A, "A" }, { Test::B, "B" } });
}

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_construct_with_helper) {
  enumeration<Test, std::string>({ { Test::A, "A" } });
}

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_construct_with_multiple_parameters_helper) {
  enumeration<Test, std::string>({ { Test::A, "A" }, { Test::B, "B" } });
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_decode) {
  const auto codec = enumeration<Test, std::string>({
      { Test::A, "A" },
      { Test::B, "B" } });
  BOOST_CHECK(test_decode(codec, "\"A\"") == Test::A);
  BOOST_CHECK(test_decode(codec, "\"B\"") == Test::B);
}

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_not_decode_invalid_value) {
  const auto codec = enumeration<Test, std::string>({ { Test::A, "A" } });
  test_decode_fail(codec, "\"B\"");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_encode) {
  const auto codec = enumeration<Test, std::string>({
      { Test::A, "A" },
      { Test::B, "B" } });
  BOOST_CHECK_EQUAL(encode(codec, Test::A), "\"A\"");
  BOOST_CHECK_EQUAL(encode(codec, Test::B), "\"B\"");
}

BOOST_AUTO_TEST_CASE(json_codec_enumeration_should_not_encode_missing_value) {
  const auto codec = enumeration<Test, std::string>({ { Test::A, "A" } });
  BOOST_CHECK_THROW(encode(codec, Test::B), encode_exception);
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
