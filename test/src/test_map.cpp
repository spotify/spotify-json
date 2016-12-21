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

#include <spotify/json/codec/any_value.hpp>
#include <spotify/json/codec/map.hpp>
#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>

#include <spotify/json/test/only_true.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename InnerType = bool>
std::map<std::string, InnerType> map_parse(const char *not_map) {
  const auto codec = default_codec<std::map<std::string, InnerType>>();
  auto ctx = decode_context(not_map, not_map + strlen(not_map));
  const auto result = codec.decode(ctx);
  BOOST_CHECK_EQUAL(ctx.position, ctx.end);;
  return result;
}

void map_parse_should_fail(const char *not_map) {
  const auto codec = default_codec<std::map<std::string, bool>>();
  auto ctx = decode_context(not_map, not_map + strlen(not_map));
  BOOST_CHECK_THROW(codec.decode(ctx), decode_exception);
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_map_should_construct_with_helper) {
  map<std::map<std::string, bool>>(boolean());
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_construct_map_with_default_codec) {
  default_codec<std::map<std::string, bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_construct_unordered_map_with_default_codec) {
  default_codec<std::unordered_map<std::string, bool>>();
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_empty_map) {
  BOOST_CHECK(map_parse("{}").empty());
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_single_element) {
  std::map<std::string, bool> map;
  map["a"] = true;
  BOOST_CHECK(map_parse(R"({"a":true})") == map);
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_two_elements) {
  std::map<std::string, bool> map;
  map["a"] = true;
  map["b"] = false;
  BOOST_CHECK(map_parse(R"({"a":true,"b":false})") == map);
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_encoded_value_element) {
  std::map<std::string, encoded_value> map;
  map["a"] = encoded_value("true");
  BOOST_CHECK(map_parse<encoded_value>(R"({"a":true})") == map);
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_not_decode_otherwise) {
  map_parse_should_fail("");
  map_parse_should_fail("{");
  map_parse_should_fail("{{");
  map_parse_should_fail(R"({"a":false)");
  map_parse_should_fail(R"({"a":false,"b":true,})");
  map_parse_should_fail(R"({"a":false,"b":true,)");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_empty) {
  std::map<std::string, bool> map;
  BOOST_CHECK_EQUAL(encode(map), "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_single_element) {
  std::map<std::string, bool> map;
  map["a"] = true;
  BOOST_CHECK_EQUAL(encode(map), R"({"a":true})");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_two_elements) {
  std::map<std::string, bool> map;
  map["a"] = true;
  map["b"] = false;
  BOOST_CHECK_EQUAL(encode(map), R"({"a":true,"b":false})");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_encoded_value_elements) {
  std::map<std::string, encoded_value> map;
  map["a"] = encoded_value("true");
  map["b"] = encoded_value("false");
  BOOST_CHECK_EQUAL(encode(map), R"({"a":true,"b":false})");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_respect_should_encode) {
  std::map<std::string, bool> map;
  map["a"] = true;
  map["b"] = false;
  const auto codec = codec::map<std::map<std::string, bool>>(only_true_t());
  BOOST_CHECK_EQUAL(encode(codec, map), R"({"a":true})");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
