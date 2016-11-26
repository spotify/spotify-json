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

void verify_decode_any_value(const std::string &json) {
  const auto codec = any_value();
  const encoded_value_ref value_ref = json::decode(codec, json);
  const encoded_value value = json::decode(codec, json);
  BOOST_CHECK_EQUAL(json, std::string(value_ref.data(), value_ref.size()));
  BOOST_CHECK_EQUAL(json, std::string(value.data(), value.size()));
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

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_value_ref_as_is) {
  const auto sdata = std::string("1234");
  const auto value = encoded_value_ref(sdata.data(), sdata.size());
  BOOST_CHECK_EQUAL(encode(value), sdata);
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_value_as_is) {
  const auto sdata = std::string("1234");
  const auto value = encoded_value(sdata);
  BOOST_CHECK_EQUAL(encode(value), sdata);
}

BOOST_AUTO_TEST_CASE(json_codec_any_value_should_encode_with_separators) {
  const auto sdata = std::string("{}");
  const auto value = encoded_value_ref(sdata.data(), sdata.size());
  const std::vector<encoded_value_ref> refs{ value, value, value };
  BOOST_CHECK_EQUAL(encode(refs), "[{},{},{}]");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
