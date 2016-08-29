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

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode_decode.hpp>

#include <spotify/json/test/only_true.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

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

struct simple_t {
  std::string value;
};

struct example_t {
  simple_t simple;
  std::string value;
};

codec::object_t<example_t> example_codec() {
  codec::object_t<example_t> codec;
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

codec::object_t<getset_t> getset_codec() {
  codec::object_t<getset_t> codec;
  codec.required("value", &getset_t::get_value, &getset_t::set_value);
  return codec;
}

codec::object_t<getset_t> getset_lambda_codec() {
  codec::object_t<getset_t> codec;
  codec.required("value",
                 [](const getset_t &x) { return x.get_value(); },
                 [](getset_t &x, const std::string &value) { x.set_value(value); });
  return codec;
}

struct subclass_t : simple_t {};

codec::object_t<subclass_t> subclass_codec() {
  codec::object_t<subclass_t> codec;
  codec.optional("value", &subclass_t::value);
  return codec;
}

}  // namespace

template <>
struct default_codec_t<simple_t> {
  static codec::object_t<simple_t> codec() {
    codec::object_t<simple_t> codec;
    codec.optional("value", &simple_t::value);
    return codec;
  }
};

BOOST_AUTO_TEST_SUITE(codec)

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_object_should_construct) {
  object_t<simple_t> codec;
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_construct_with_custom_creator) {
  object_t<example_t> codec([]{
    return example_t();
  });
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_fields) {
  const auto simple = test_decode(default_codec<simple_t>(), R"({"value":"hey"})");
  BOOST_CHECK_EQUAL(simple.value, "hey");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_support_omitting_optional_fields) {
  const auto example = test_decode(example_codec(), R"({"value":"hey"})");
  BOOST_CHECK_EQUAL(example.value, "hey");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_require_required_fields) {
  test_decode_fail(example_codec(), "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_overwrite_duplicate_fields) {
  // The JSON spec doesn't say what to do in this case. This test simply verifies that it does
  // something that makes sense somehow.
  const auto example = test_decode(example_codec(), R"({"value":"hey1","value":"hey2"})");
  BOOST_CHECK_EQUAL(example.value, "hey2");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_use_custom_creator_when_decoding) {
  object_t<example_t> codec([]{
    example_t value;
    value.value = "hello";
    return value;
  });
  const auto example = test_decode(codec, "{}");
  BOOST_CHECK_EQUAL(example.value, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_use_custom_creator_when_decoding_factory) {
  auto codec = object([]{
    example_t value;
    value.value = "hello";
    return value;
  });
  const auto example = test_decode(codec, "{}");
  BOOST_CHECK_EQUAL(example.value, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_use_provided_codec) {
  object_t<simple_t> other_simple_codec;
  other_simple_codec.optional("other", &simple_t::value);

  object_t<example_t> codec;
  codec.required("s", &example_t::simple, other_simple_codec);
  const auto example = test_decode(codec, R"({"s":{"other":"Hello!"}})");
  BOOST_CHECK_EQUAL(example.simple.value, "Hello!");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_dummy_fields) {
  object_t<example_t> codec;
  codec.required("dummy", boolean());
  test_decode(codec, R"({"dummy":true})");
  test_decode_fail(codec, R"({"dummy":null})");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_setter_field) {
  const auto codec = getset_codec();
  const auto getset = test_decode(codec, R"({"value":"foobar"})");
  BOOST_CHECK_EQUAL("foobar", getset.get_value());
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_decode_lambda_setter_field) {
  const auto codec = getset_lambda_codec();
  const auto getset = test_decode(codec, R"({"value":"foobar"})");
  BOOST_CHECK_EQUAL("foobar", getset.get_value());
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_work_with_base_class_member_ptr) {
  const auto codec = subclass_codec();
  subclass_t subclass;
  subclass.value = "foobar";

  BOOST_CHECK_EQUAL(decode(codec, encode(codec, subclass)).value, subclass.value);
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_fields) {
  simple_t simple;
  simple.value = "hey";
  BOOST_CHECK_EQUAL(encode(simple), R"({"value":"hey"})");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_respect_should_encode) {
  using data_t = std::pair<bool, bool>;
  const auto data = data_t(true, false);

  codec::object_t<data_t> codec;
  codec.optional("first", &data_t::first, only_true_t());
  codec.required("second", &data_t::second, only_true_t());

  BOOST_CHECK_EQUAL(encode(codec, data), R"({"first":true})");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_fields_in_provided_order) {
  simple_t simple;
  simple.value = "";

  codec::object_t<simple_t> codec;
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

  const auto answer = "{"
      R"("0":"","1":"","2":"","3":"","4":"",)"
      R"("5":"","6":"","7":"","8":"","9":""})";
  BOOST_CHECK_EQUAL(encode(codec, simple), answer);
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_dummy_fields) {
  object_t<example_t> codec;
  codec.required("dummy", string());

  BOOST_CHECK_EQUAL(encode(codec, example_t()), R"({"dummy":""})");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_getter_field) {
  const auto codec = getset_codec();
  getset_t getset;
  getset.set_value("foobar");

  BOOST_CHECK_EQUAL(encode(codec, getset), R"({"value":"foobar"})");
}

BOOST_AUTO_TEST_CASE(json_codec_object_should_encode_lambda_getter_field) {
  const auto codec = getset_lambda_codec();
  getset_t getset;
  getset.set_value("foobar");

  BOOST_CHECK_EQUAL(encode(codec, getset), R"({"value":"foobar"})");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
