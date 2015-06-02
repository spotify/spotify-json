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

#include <spotify/json/codec/map.hpp>
#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

std::map<std::string, bool> map_parse(const char *not_map) {
  const auto codec = standard<std::map<std::string, bool>>();
  auto ctx = decoding_context(not_map, not_map + strlen(not_map));

  const auto result = codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  BOOST_CHECK(!ctx.has_failed());

  return result;
}

void map_parse_should_fail(const char *not_map) {
  const auto codec = standard<std::map<std::string, bool>>();
  auto ctx = decoding_context(not_map, not_map + strlen(not_map));
  const auto original_ctx = ctx;

  codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
  BOOST_CHECK(ctx.has_failed());
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_empty_map) {
  BOOST_CHECK(map_parse("{}").empty());
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_single_element) {
  std::map<std::string, bool> map;
  map["a"] = true;
  BOOST_CHECK(map_parse("{\"a\":true}") == map);
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_decode_two_elements) {
  std::map<std::string, bool> map;
  map["a"] = true;
  map["b"] = false;
  BOOST_CHECK(map_parse("{\"a\":true,\"b\":false}") == map);
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_not_decode_otherwise) {
  map_parse_should_fail("");
  map_parse_should_fail("{");
  map_parse_should_fail("{{");
  map_parse_should_fail("{\"a\":false");
  map_parse_should_fail("{\"a\":false,\"b\":true,}");
  map_parse_should_fail("{\"a\":false,\"b\":true,");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_empty) {
  std::map<std::string, bool> map;
  BOOST_CHECK_EQUAL(encode(map), "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_single_element) {
  std::map<std::string, bool> map;
  map["a"] = true;
  BOOST_CHECK_EQUAL(encode(map), "{\"a\":true}");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_encode_two_elements) {
  std::map<std::string, bool> map;
  map["a"] = true;
  map["b"] = false;
  BOOST_CHECK_EQUAL(encode(map), "{\"a\":true,\"b\":false}");
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_construct_with_helper) {
  map<std::map<std::string, bool>>(boolean());
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_construct_map_with_standard) {
  standard<std::map<std::string, bool>>();
}

BOOST_AUTO_TEST_CASE(json_codec_map_should_construct_unordered_map_with_standard) {
  standard<std::unordered_map<std::string, bool>>();
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
