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
#include <spotify/json/codec/transform.hpp>
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
std::string test_encode(const Codec &codec, const typename Codec::object_type &value) {
  encoding_context c;
  codec.encode(c, value);
  const auto data = c.data();
  return std::string(data, data + c.size());
}

struct my_type {
  std::string value;
};

std::string encodeTransform(const my_type &object) {
  return object.value;
}

my_type decodeTransform(const std::string &value, size_t where) {
  return my_type{ value };
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_transform_should_construct) {
  transform_t<string_t, decltype(&encodeTransform), decltype(&decodeTransform)> codec(
      string(), &encodeTransform, &decodeTransform);
}

BOOST_AUTO_TEST_CASE(json_codec_transform_should_construct_with_helper_with_codec) {
  transform(string(), &encodeTransform, &decodeTransform);
}

BOOST_AUTO_TEST_CASE(json_codec_transform_should_construct_with_helper) {
  transform(&encodeTransform, &decodeTransform);
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_transform_should_decode) {
  const auto codec = transform(&encodeTransform, &decodeTransform);
  const my_type result = test_decode(codec, "\"A\"");
  BOOST_CHECK_EQUAL(result.value, "A");
}

BOOST_AUTO_TEST_CASE(json_codec_transform_should_provide_position_to_decode) {
  const auto decode_transform = [](const std::string &value, size_t where) {
    return my_type{ std::to_string(where) };
  };

  const auto codec = array<std::vector<my_type>>(
      transform(&encodeTransform, decode_transform));
  BOOST_CHECK_EQUAL(test_decode(codec, R"([  "A"])")[0].value, "3");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_transforms_should_encode) {
  const auto codec = transform(&encodeTransform, &decodeTransform);
  BOOST_CHECK_EQUAL(encode(codec, my_type{ "A" }), "\"A\"");
  BOOST_CHECK_EQUAL(test_encode(codec, my_type{ "A" }), "\"A\"");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
