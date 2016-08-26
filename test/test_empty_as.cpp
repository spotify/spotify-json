/*
 * Copyright (c) 2016 Spotify AB
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

#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/empty_as.hpp>
#include <spotify/json/codec/eq.hpp>
#include <spotify/json/codec/null.hpp>
#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/omit.hpp>
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

struct Val {
  std::string a;
  std::string b;
};

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

template <typename Codec>
std::string test_encode(const Codec &codec, const typename Codec::object_type &value) {
  encoding_context c;
  codec.encode(c, value);
  const auto data = c.data();
  return std::string(data, data + c.size());
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_default_construct) {
  empty_as_t<null_t<std::string>, string_t> codec;
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_construct) {
  empty_as_t<null_t<std::string>, string_t> codec((null_t<std::string>()), string_t());
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_construct_with_helper) {
  empty_as(null<std::string>(), string());
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_null_should_construct) {
  empty_as_null(string());
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_omit_should_construct) {
  empty_as_omit(string());
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_decode) {
  const auto codec = empty_as_omit(string());
  BOOST_CHECK_EQUAL(test_decode(codec, "\"\""), "");
  BOOST_CHECK_EQUAL(test_decode(codec, "\"hello\""), "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_decode_default) {
  const auto codec = empty_as_null(string());
  BOOST_CHECK_EQUAL(test_decode(codec, "null"), "");
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_fail_on_invalid_input) {
  const auto codec = empty_as_omit(string());
  test_decode_fail(codec, "null");
  test_decode_fail(codec, "e");
  test_decode_fail(codec, "[{},true]");
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_with_eq) {
  const auto codec = empty_as(eq(123), number<int>());
  BOOST_CHECK_EQUAL(encode(codec, 0), "123");
  BOOST_CHECK(test_decode(codec, "123") == 123);
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_encode_default) {
  const auto codec = empty_as_null(string());
  BOOST_CHECK(codec.should_encode(""));
  BOOST_CHECK_EQUAL(encode(codec, ""), "null");
  BOOST_CHECK_EQUAL(test_encode(codec, ""), "null");
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_not_encode_default) {
  const auto codec = empty_as_omit(string());
  BOOST_CHECK(!codec.should_encode(""));
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_should_encode_non_default) {
  const auto codec = empty_as_omit(string());
  BOOST_CHECK(codec.should_encode("abc"));
  BOOST_CHECK_EQUAL(encode(codec, "abc"), "\"abc\"");
  BOOST_CHECK_EQUAL(test_encode(codec, "abc"), "\"abc\"");
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_with_shared_ptr) {
  const auto codec = empty_as_null(default_codec<std::shared_ptr<std::string>>());
  BOOST_CHECK_EQUAL(encode(codec, std::shared_ptr<std::string>()), "null");
  BOOST_CHECK_EQUAL(encode(codec, std::make_shared<std::string>("abc")), "\"abc\"");
  BOOST_CHECK_EQUAL(test_encode(codec, std::shared_ptr<std::string>()), "null");
  BOOST_CHECK_EQUAL(test_encode(codec, std::make_shared<std::string>("abc")), "\"abc\"");
}

BOOST_AUTO_TEST_CASE(json_codec_empty_as_with_object_and_omit) {
  auto codec = object<Val>();
  codec.optional("a", &Val::a);
  codec.optional("b", &Val::b, empty_as_omit(string()));
  BOOST_CHECK_EQUAL(encode(codec, Val()), R"({"a":""})");  // no "b"
  BOOST_CHECK_EQUAL(test_encode(codec, Val()), R"({"a":""})");  // no "b"
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
