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

#include <spotify/json/codec/ignore.hpp>
#include <spotify/json/codec/null.hpp>
#include <spotify/json/codec/one_of.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename Codec>
typename Codec::object_type test_decode(const Codec &codec, const std::string &json) {
  decode_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK_EQUAL(c.position, c.end);
  return obj;
}

template <typename Codec>
void test_decode_fail(const Codec &codec, const std::string &json) {
  decode_context c(json.c_str(), json.c_str() + json.size());
  BOOST_CHECK_THROW(codec.decode(c), decode_exception);
}

struct example_t {
  std::string value;
};

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_construct) {
  one_of_t<object_t<example_t>> codec;
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_construct_with_helper) {
  one_of(object<example_t>(), object<example_t>());
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_decode_with_first_if_possible) {
  auto first = object<example_t>();
  first.required("a", &example_t::value);

  auto second = object<example_t>();

  const auto codec = one_of(first, second);
  const auto example = test_decode(codec, R"({"a":"first"})");
  BOOST_CHECK_EQUAL(example.value, "first");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_decode_with_second_if_needed) {
  auto first = object<example_t>();
  first.required("a", &example_t::value);

  auto second = object<example_t>();
  second.required("b", &example_t::value);

  const auto codec = one_of(first, second);
  const auto example = test_decode(codec, R"({"b":"second"})");
  BOOST_CHECK_EQUAL(example.value, "second");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_fail_decode_if_all_fail) {
  auto first = object<example_t>();
  first.required("a", &example_t::value);

  auto second = object<example_t>();
  second.required("b", &example_t::value);

  const auto codec = one_of(first, second);
  test_decode_fail(codec, "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_null) {
  const auto codec = one_of(string(), null<std::string>());
  BOOST_CHECK_EQUAL(test_decode(codec, "\"abc\""), "abc");
  BOOST_CHECK_EQUAL(test_decode(codec, "\"\""), "");
  BOOST_CHECK_EQUAL(test_decode(codec, "null"), "");
  test_decode_fail(codec, "{}");
  test_decode_fail(codec, "{");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_ignore) {
  const auto codec = one_of(string(), ignore<std::string>());
  BOOST_CHECK_EQUAL(test_decode(codec, "\"abc\""), "abc");
  BOOST_CHECK_EQUAL(test_decode(codec, "\"\""), "");
  BOOST_CHECK_EQUAL(test_decode(codec, "null"), "");
  BOOST_CHECK_EQUAL(test_decode(codec, "{}"), "");
  test_decode_fail(codec, "{");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_encode_with_first) {
  auto first = object<example_t>();
  first.required("a", &example_t::value);

  auto second = object<example_t>();
  second.required("b", &example_t::value);

  const auto codec = one_of(first, second);

  example_t value;
  value.value = "val";

  BOOST_CHECK_EQUAL(encode(codec, value), R"({"a":"val"})");
}

BOOST_AUTO_TEST_CASE(json_codec_should_forward_should_encode) {
  const auto yay = one_of(string(), ignore<std::string>());
  const auto nay = one_of(ignore<std::string>(), string());
  BOOST_CHECK(yay.should_encode(""));
  BOOST_CHECK(!nay.should_encode(""));
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
