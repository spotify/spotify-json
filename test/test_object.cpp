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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode_decode.hpp>

#include "only_true.hpp"

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

template<typename Codec>
typename Codec::object_type test_decode(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK_EQUAL(c.position, c.end);
  return obj;
}

template<typename Codec>
void test_decode_fail(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  BOOST_CHECK_THROW(codec.decode(c), decode_exception);
}

struct simple_t {
  std::string value;
};

struct example_t {
  simple_t simple;
  std::string value;
};

codec::object<example_t> example_codec() {
  codec::object<example_t> codec;
  codec.optional("simple", &example_t::simple);
  codec.required("value", &example_t::value);
  return codec;
}

class getset_t {
 public:
  const std::string &get_value() const { return value; }
  void set_value(const std::string &v) { value = v; }

 private:
  std::string value;
};

codec::object<getset_t> getset_codec() {
  codec::object<getset_t> codec;
  codec.required("value", &getset_t::get_value, &getset_t::set_value);
  return codec;
}

codec::object<getset_t> getset_lambda_codec() {
  codec::object<getset_t> codec;
  codec.required("value",
                 [](const getset_t &x) { return x.get_value(); },
                 [](getset_t &x, const std::string &value) { x.set_value(value); });
  return codec;
}

}  // namespace

template<>
struct default_codec_t<simple_t> {
  static codec::object<simple_t> codec() {
    codec::object<simple_t> codec;
    codec.optional("value", &simple_t::value);
    return codec;
  }
};

BOOST_AUTO_TEST_SUITE(codec)

BOOST_AUTO_TEST_CASE(json_codec_object_should_construct) {
  object<simple_t> codec;
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_construct_with_custom_creator) {
  object<example_t> codec([]{
    return example_t();
  });
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_fields) {
  const auto simple = test_decode(default_codec<simple_t>(), "{\"value\":\"hey\"}");
  BOOST_CHECK_EQUAL(simple.value, "hey");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_support_omitting_optional_fields) {
  const auto example = test_decode(example_codec(), "{\"value\":\"hey\"}");
  BOOST_CHECK_EQUAL(example.value, "hey");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_require_required_fields) {
  test_decode_fail(example_codec(), "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_overwrite_duplicate_fields) {
  // The JSON spec doesn't say what to do in this case. This test simply verifies that it does
  // something that makes sense somehow.
  const auto example = test_decode(example_codec(), "{\"value\":\"hey1\",\"value\":\"hey2\"}");
  BOOST_CHECK_EQUAL(example.value, "hey2");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_use_custom_creator_when_decoding) {
  object<example_t> codec([]{
    example_t value;
    value.value = "hello";
    return value;
  });
  const auto example = test_decode(codec, "{}");
  BOOST_CHECK_EQUAL(example.value, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_fields) {
  simple_t simple;
  simple.value = "hey";
  const auto json = encode(simple);
  BOOST_CHECK_EQUAL(json, "{\"value\":\"hey\"}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_respect_should_encode) {
  using data_t = std::pair<bool, bool>;
  const auto data = data_t(true, false);

  codec::object<data_t> codec;
  codec.optional("first", &data_t::first, only_true_t());
  codec.required("second", &data_t::second, only_true_t());

  BOOST_CHECK_EQUAL(encode(codec, data), "{\"first\":true}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_fields_in_provided_order) {
  simple_t simple;
  simple.value = "";

  codec::object<simple_t> codec;
  codec.required("0", &simple_t::value);
  codec.optional("1", &simple_t::value);
  codec.optional("2", &simple_t::value);
  codec.optional("3", &simple_t::value);
  codec.optional("4", &simple_t::value);
  codec.optional("5", &simple_t::value);
  codec.optional("6", &simple_t::value);
  codec.optional("7", &simple_t::value);
  codec.optional("8", &simple_t::value);
  codec.optional("9", &simple_t::value);

  BOOST_CHECK_EQUAL(encode(codec, simple), "{"
      "\"0\":\"\",\"1\":\"\",\"2\":\"\",\"3\":\"\",\"4\":\"\","
      "\"5\":\"\",\"6\":\"\",\"7\":\"\",\"8\":\"\",\"9\":\"\"}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_use_provided_codec) {
  object<simple_t> other_simple_codec;
  other_simple_codec.optional("other", &simple_t::value);

  object<example_t> codec;
  codec.required("s", &example_t::simple, other_simple_codec);
  const auto example = test_decode(codec, "{\"s\":{\"other\":\"Hello!\"}}");
  BOOST_CHECK_EQUAL(example.simple.value, "Hello!");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_dummy_fields) {
  object<example_t> codec;
  codec.required("dummy", boolean());

  test_decode(codec, "{\"dummy\":true}");
  test_decode_fail(codec, "{\"dummy\":null}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_dummy_fields) {
  object<example_t> codec;
  codec.required("dummy", string());

  BOOST_CHECK_EQUAL(encode(codec, example_t()), "{\"dummy\":\"\"}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_setter_field) {
  const auto codec = getset_codec();
  const auto getset = test_decode(codec, "{\"value\":\"foobar\"}");

  BOOST_CHECK_EQUAL("foobar", getset.get_value());
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_getter_field) {
  const auto codec = getset_codec();
  getset_t getset;
  getset.set_value("foobar");

  BOOST_CHECK_EQUAL(encode(codec, getset), "{\"value\":\"foobar\"}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_lambda_setter_field) {
  const auto codec = getset_lambda_codec();
  const auto getset = test_decode(codec, "{\"value\":\"foobar\"}");

  BOOST_CHECK_EQUAL("foobar", getset.get_value());
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_lambda_getter_field) {
  const auto codec = getset_lambda_codec();
  getset_t getset;
  getset.set_value("foobar");

  BOOST_CHECK_EQUAL(encode(codec, getset), "{\"value\":\"foobar\"}");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify