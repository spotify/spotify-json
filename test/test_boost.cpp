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

#include <spotify/json/boost.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/encode_decode.hpp>

#include "only_true.hpp"

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

class base_class {
 public:
  virtual ~base_class() = default;
};

class sub_class : public base_class {
};

codec::object_t<sub_class> sub_codec() {
  return codec::object<sub_class>();
}

}  // namespace


/// boost::shared_ptr

BOOST_AUTO_TEST_CASE(json_codec_boost_shared_ptr_should_decode) {
  const auto obj = decode<boost::shared_ptr<std::string>>("\"hello\"");
  BOOST_REQUIRE(obj);
  BOOST_CHECK_EQUAL(*obj, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_boost_cast_pointer_should_construct_with_helper) {
  const boost::shared_ptr<base_class> ptr = boost::make_shared<sub_class>();
  const auto codec = codec::cast<boost::shared_ptr<base_class>>(boost_shared_ptr(sub_codec()));
  BOOST_CHECK_EQUAL(encode(codec, ptr), "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_boost_shared_ptr_should_not_encode_null) {
  const auto codec = boost_shared_ptr(codec::string());
  boost::shared_ptr<std::string> obj;
  BOOST_CHECK(!detail::should_encode(codec, obj));
}

/// boost::optional

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_construct) {
  const auto c = codec::optional_t<codec::string_t>(codec::string());
  static_cast<void>(c);
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_construct_none_as_null) {
  const auto c = codec::optional_t<codec::string_t>(codec::string(), codec::none_as_null);
  static_cast<void>(c);
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_construct_with_helper) {
  const auto c = codec::optional((codec::string()));
  static_cast<void>(c);
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_construct_with_helper_with_none_as_null) {
  const auto c = codec::optional(codec::string(), codec::none_as_null);
  static_cast<void>(c);
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_construct_with_default_codec) {
  default_codec<boost::optional<std::string>>();
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_encode) {
  const auto codec = default_codec<boost::optional<std::string>>();
  BOOST_CHECK_EQUAL(encode(codec, boost::make_optional(std::string("hi"))), "\"hi\"");
  BOOST_CHECK_EQUAL(encode(codec, boost::none), "null");
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_decode) {
  const auto codec = default_codec<boost::optional<std::string>>();
  BOOST_CHECK(decode(codec, "\"hi\"") == boost::make_optional(std::string("hi")));

  boost::optional<std::string> value;
  BOOST_CHECK(!try_decode(value, codec, "\"hi"));
  BOOST_CHECK(!try_decode(value, codec, ""));
  BOOST_CHECK(!try_decode(value, codec, "5"));
  // This should only work if none_as_null = true
  BOOST_CHECK(!try_decode(value, codec, "null"));
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_decode_null_as_none) {
  const auto codec = codec::optional(codec::string(), codec::none_as_null);
  BOOST_CHECK(decode(codec, "null") == boost::optional<std::string>());

  boost::optional<std::string> value;
  BOOST_CHECK(!try_decode(value, codec, "nul"));
  BOOST_CHECK(!try_decode(value, codec, "nuff"));
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_implement_should_encode) {
  const auto codec = default_codec<boost::optional<std::string>>();
  BOOST_CHECK(codec.should_encode(boost::make_optional(std::string(""))));
  BOOST_CHECK(!codec.should_encode(boost::none));
}

BOOST_AUTO_TEST_CASE(json_codec_boost_optional_should_encode_returns_true_if_none_as_null) {
  const auto codec = codec::optional(codec::string(), codec::none_as_null);
  BOOST_CHECK(codec.should_encode(boost::none));
}

/// boost::chrono

BOOST_AUTO_TEST_CASE(json_codec_duration_should_construct) {
  codec::duration<boost::chrono::system_clock::duration>();
  codec::duration<boost::chrono::steady_clock::duration>();
  codec::duration<boost::chrono::high_resolution_clock::duration>();
}

BOOST_AUTO_TEST_CASE(json_codec_duration_should_construct_with_default_codec) {
  default_codec<boost::chrono::system_clock::duration>();
  default_codec<boost::chrono::steady_clock::duration>();
  default_codec<boost::chrono::high_resolution_clock::duration>();
}

BOOST_AUTO_TEST_CASE(json_codec_duration_should_encode) {
  BOOST_CHECK_EQUAL(encode(boost::chrono::system_clock::duration(5)), "5");
}

BOOST_AUTO_TEST_CASE(json_codec_duration_should_decode) {
  BOOST_CHECK(
      decode<boost::chrono::system_clock::duration>("5") ==
      boost::chrono::system_clock::duration(5));
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_construct) {
  codec::time_point<boost::chrono::system_clock::time_point>();
  codec::time_point<boost::chrono::steady_clock::time_point>();
  codec::time_point<boost::chrono::high_resolution_clock::time_point>();
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_construct_with_default_codec) {
  default_codec<boost::chrono::system_clock::time_point>();
  default_codec<boost::chrono::steady_clock::time_point>();
  default_codec<boost::chrono::high_resolution_clock::time_point>();
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_encode) {
  using boost::chrono::system_clock;
  BOOST_CHECK_EQUAL(encode(system_clock::time_point(system_clock::duration(5))), "5");
}

BOOST_AUTO_TEST_CASE(json_codec_time_point_should_decode) {
  using boost::chrono::system_clock;
  BOOST_CHECK(
      decode<system_clock::time_point>("5") ==
      system_clock::time_point(system_clock::duration(5)));
}

/// boost::container::flat_map

BOOST_AUTO_TEST_CASE(json_codec_flat_map_should_decode) {
  BOOST_CHECK((decode<boost::container::flat_map<std::string, int>>("{\"foo\":1234}")) ==
              (boost::container::flat_map<std::string, int>{{"foo", 1234}}));
}

BOOST_AUTO_TEST_CASE(json_codec_flat_map_should_encode) {
  BOOST_CHECK_EQUAL((encode(boost::container::flat_map<std::string, int>{{"foo", 1234}})),
                    "{\"foo\":1234}");
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
