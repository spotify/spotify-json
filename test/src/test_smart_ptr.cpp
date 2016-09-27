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
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename codec_type>
typename codec_type::object_type test_decode(const codec_type &codec, const std::string &json) {
  decode_context context(json.c_str(), json.c_str() + json.size());
  auto object = codec.decode(context);
  BOOST_CHECK_EQUAL(context.position, context.end);
  return object;
}

}  // namespace

/*
 * std::unique_ptr
 */

BOOST_AUTO_TEST_CASE(json_codec_unique_ptr_should_construct) {
  unique_ptr_t<string_t> codec(string());
}

BOOST_AUTO_TEST_CASE(json_codec_unique_ptr_should_construct_with_helper) {
  unique_ptr(string());
}

BOOST_AUTO_TEST_CASE(json_codec_unique_ptr_should_construct_with_default_codec) {
  default_codec_t<std::unique_ptr<std::string>>();
}

BOOST_AUTO_TEST_CASE(json_codec_unique_ptr_should_encode) {
  const auto codec = unique_ptr(string());
  const auto input = std::unique_ptr<std::string>(new std::string("hello"));
  BOOST_CHECK_EQUAL(encode(codec, input), "\"hello\"");
}

BOOST_AUTO_TEST_CASE(json_codec_unique_ptr_should_decode) {
  const auto codec = unique_ptr(string());
  std::unique_ptr<std::string> obj = test_decode(codec, "\"hello\"");
  BOOST_REQUIRE(obj);
  BOOST_CHECK_EQUAL(*obj, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_unique_ptr_should_not_encode_null) {
  const auto codec = unique_ptr(string());
  std::unique_ptr<std::string> obj;
  BOOST_CHECK(!detail::should_encode(codec, obj));
}

/*
 * std::shared_ptr
 */

BOOST_AUTO_TEST_CASE(json_codec_shared_ptr_should_construct) {
  shared_ptr_t<string_t> codec(string());
}

BOOST_AUTO_TEST_CASE(json_codec_shared_ptr_should_construct_with_helper) {
  shared_ptr(string());
}

BOOST_AUTO_TEST_CASE(json_codec_shared_ptr_should_construct_with_default_codec) {
  default_codec_t<std::shared_ptr<std::string>>();
}

BOOST_AUTO_TEST_CASE(json_codec_shared_ptr_should_encode) {
  const auto codec = shared_ptr(string());
  const auto input = std::make_shared<std::string>("hello");
  BOOST_CHECK_EQUAL(encode(codec, input), "\"hello\"");
}

BOOST_AUTO_TEST_CASE(json_codec_shared_ptr_should_decode) {
  const auto codec = shared_ptr(string());
  std::shared_ptr<std::string> obj = test_decode(codec, "\"hello\"");
  BOOST_REQUIRE(obj);
  BOOST_CHECK_EQUAL(*obj, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_shared_ptr_should_not_encode_null) {
  const auto codec = shared_ptr(string());
  std::shared_ptr<std::string> obj;
  BOOST_CHECK(!detail::should_encode(codec, obj));
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
