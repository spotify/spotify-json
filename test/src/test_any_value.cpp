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

#include <cstdint>
#include <cstdlib>
#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/any_value.hpp>
#include <spotify/json/codec/array.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename value_type>
struct foobar_t {
  value_type value;
};

template <typename value_type = ref>
void verify_decode_any_value(const std::string &json) {
  const auto codec = any_value<value_type>();
  const auto decoded = json::decode(codec, json);
  BOOST_CHECK_EQUAL(json, std::string(decoded.data(), decoded.data() + decoded.size()));
}

}  // namespace

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_array) {
  verify_decode_any_value("[1, 2, 3]");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_object) {
  verify_decode_any_value(R"({"hey":"yo"})");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_boolean) {
  verify_decode_any_value("true");
  verify_decode_any_value("false");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_null) {
  verify_decode_any_value("null");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_string) {
  verify_decode_any_value("\"foobar\"");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_number) {
  verify_decode_any_value("123");
  verify_decode_any_value("123.456");
  verify_decode_any_value("-123.456");
  verify_decode_any_value("-123.456e+45");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_deep_json) {
  // This is deep enough to blow the stack if the any_value codec is implemented
  // using simple recursion. The failure case of this test is that it crashes.
  const auto depth = 1000000;

  std::string str;
  str.reserve(depth * 2);
  for (auto i = 0; i < depth; i++) { str += '['; }
  for (auto i = 0; i < depth; i++) { str += ']'; }

  verify_decode_any_value(str);
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_into_string) {
  verify_decode_any_value<std::string>("[1, 2, 3]");
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_decode_into_vector) {
  verify_decode_any_value<std::vector<uint8_t>>("[1, 2, 3]");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_ref_as_is) {
  std::string data = "1234";
  ref data_ref(data.data(), data.size());
  BOOST_CHECK_EQUAL(encode(data_ref), data);
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_string_as_is) {
  std::string data = "1234";
  BOOST_CHECK_EQUAL(encode(any_value<std::string>(), data), data);
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_vector_as_is) {
  std::string data = "1234";
  std::vector<uint8_t> vec(data.data(), data.data() + data.size());
  BOOST_CHECK_EQUAL(encode(any_value<std::vector<uint8_t>>(), vec), data);
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_with_separators) {
  std::string raw = "{}";
  ref data_ref(raw.data(), raw.size());
  std::vector<ref> refs{data_ref, data_ref, data_ref};
  BOOST_CHECK_EQUAL(encode(refs), "[{},{},{}]");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
