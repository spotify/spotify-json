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

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/array.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/raw.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode_decode.hpp>

namespace {

struct foobar_t {
  spotify::json::codec::raw_ref value;
};

}  // namespace

namespace spotify {
namespace json {

template<>
struct default_codec_t<foobar_t> {
  static codec::object_t<foobar_t> codec() {
    auto codec = codec::object<foobar_t>();
    codec.required("value", &foobar_t::value);
    return codec;
  }
};

}  // namespace json
}  // namespace spotify

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

foobar_t decode_foobar(const char *data) {
  decoding_context ctx(data, data + std::strlen(data));
  return default_codec<foobar_t>().decode(ctx);
}

void verify_decode_raw(const std::string &raw_value) {
  std::string data;
  data += "{\"foo\": 123,\"value\": ";
  const auto expected_begin = data.size();
  data += raw_value;
  data += ",\"bar\": 456}";

  const auto foobar = decode_foobar(data.c_str());
  BOOST_CHECK_EQUAL(expected_begin, foobar.value.data - data.c_str());
  BOOST_CHECK_EQUAL(raw_value.size(), foobar.value.size);
}

}  // namespace

//
// Decoding
//

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_array) {
  verify_decode_raw("[1, 2, 3]");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_object) {
  verify_decode_raw("{\"hey\":\"yo\"}");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_boolean) {
  verify_decode_raw("true");
  verify_decode_raw("false");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_null) {
  verify_decode_raw("null");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_string) {
  verify_decode_raw("\"foobar\"");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_number) {
  verify_decode_raw("123");
  verify_decode_raw("123.456");
  verify_decode_raw("-123.456");
  verify_decode_raw("-123.456e+45");
}

//
// Encoding
//

BOOST_AUTO_TEST_CASE(json_codec_raw_should_encode_ref_as_is) {
  buffer buffer;
  detail::writer writer(buffer);
  std::string data = "some junk";
  raw_ref ref(data.data(), data.size());
  raw().encode(ref, writer);

  BOOST_CHECK_EQUAL(data, std::string(buffer.data(), buffer.size()));
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_encode_with_separators) {
  std::string raw = "{}";
  raw_ref ref(raw.data(), raw.size());
  std::vector<raw_ref> refs{ref, ref, ref};

  BOOST_CHECK_EQUAL(encode(refs), "[{},{},{}]");
}

//
// Raw ref
//

BOOST_AUTO_TEST_CASE(json_codec_raw_ref_should_construct_from_data_size) {
  std::string raw = "true";
  raw_ref ref(raw.data(), raw.size());

  BOOST_CHECK_EQUAL(ref.data, raw.data());
  BOOST_CHECK_EQUAL(ref.size, raw.size());
}

BOOST_AUTO_TEST_CASE(json_codec_raw_ref_should_construct_from_begin_end) {
  std::string raw = "true";
  raw_ref ref(raw.data(), raw.data() + raw.size());

  BOOST_CHECK_EQUAL(ref.data, raw.data());
  BOOST_CHECK_EQUAL(ref.size, raw.size());
}

BOOST_AUTO_TEST_CASE(json_codec_raw_ref_should_convert_to_decoding_context) {
  std::string raw = "true";
  raw_ref ref(raw.data(), raw.size());
  decoding_context context(ref);

  const auto begin = raw.data();
  const auto end = begin + raw.size();
  BOOST_CHECK_EQUAL(context.begin, begin);
  BOOST_CHECK_EQUAL(context.position, begin);
  BOOST_CHECK_EQUAL(context.end, end);
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
