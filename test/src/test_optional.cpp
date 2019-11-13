/*
 * Copyright (c) 2019 Spotify AB
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

#include <optional>
#include <string>
#include <spotify/json/codec/any_value.hpp>
#include <spotify/json/codec/optional.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/omit.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>
#include <spotify/json/encoded_value.hpp>
#include <spotify/json/test/only_true.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

struct optional_value_ref { std::optional<encoded_value_ref> value = encoded_value_ref("{}"); };

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_optional_should_construct) {
  const auto c = codec::optional_t<codec::string_t>(codec::string());
  static_cast<void>(c);
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_construct_with_helper) {
  const auto c = codec::optional((codec::string()));
  static_cast<void>(c);
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_construct_with_default_codec) {
  default_codec<std::optional<std::string>>();
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_encode_string) {
  const auto codec = default_codec<std::optional<std::string>>();
  BOOST_CHECK_EQUAL(encode(codec, std::make_optional(std::string("hi"))), "\"hi\"");
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_encode_size_t) {
  const auto codec = default_codec<std::optional<size_t>>();
  BOOST_CHECK_EQUAL(encode(codec, std::make_optional(size_t(123456))), "123456");
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_decode) {
  const auto codec = default_codec<std::optional<std::string>>();
  BOOST_CHECK(decode(codec, "\"hi\"") == std::make_optional(std::string("hi")));

  std::optional<std::string> value;
  BOOST_CHECK(!try_decode(value, codec, "\"hi"));
  BOOST_CHECK(!try_decode(value, codec, ""));
  BOOST_CHECK(!try_decode(value, codec, "5"));
  BOOST_CHECK(!try_decode(value, codec, "null"));
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_implement_should_encode) {
  const auto codec = default_codec<std::optional<std::string>>();
  BOOST_CHECK(codec.should_encode(std::make_optional(std::string(""))));
  BOOST_CHECK(!codec.should_encode(std::nullopt));
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_forward_should_encode) {
  const auto codec = codec::optional_t<codec::omit_t<std::string>>(codec::omit<std::string>());
  BOOST_CHECK(!codec.should_encode(std::make_optional(std::string(""))));
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_accept_encoded_value_ref) {
  const auto value = std::optional<encoded_value_ref>(encoded_value_ref("{}"));
  const auto codec = default_codec<std::optional<encoded_value_ref>>();
  BOOST_CHECK(detail::should_encode(codec, value));
  BOOST_CHECK(encode(value) == "{}");
  BOOST_CHECK(decode(codec, "{}").value() == value.value());
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_accept_encoded_value) {
  const auto value = std::optional<encoded_value>(encoded_value("{}"));
  const auto codec = default_codec<std::optional<encoded_value>>();
  BOOST_CHECK(detail::should_encode(codec, value));
  BOOST_CHECK(encode(value) == "{}");
  BOOST_CHECK(decode(codec, "{}").value() == value.value());
}

BOOST_AUTO_TEST_CASE(json_codec_optional_should_accept_encoded_value_ref_in_object) {
  codec::object_t<optional_value_ref> codec;
  codec.optional("value", &optional_value_ref::value);

  const optional_value_ref value{};
  BOOST_CHECK(detail::should_encode(codec, value));
  BOOST_CHECK(encode(codec, value) == "{\"value\":{}}");
  BOOST_CHECK(decode(codec, "{\"value\":{}}").value.value() == value.value.value());
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
