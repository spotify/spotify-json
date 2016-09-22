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
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/number.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode.hpp>
#include <spotify/json/encode_exception.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename Codec>
typename Codec::object_type test_decode_dont_gobble(
    const Codec &codec,
    const std::string &json,
    const size_t expected_length) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK_EQUAL(c.position, c.begin + expected_length);
  return obj;
}

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

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_number_should_construct) {
  number_t<double>();
  number_t<float>();
  number_t<int8_t>();
  number_t<int16_t>();
  number_t<int32_t>();
  number_t<int64_t>();
  number_t<uint8_t>();
  number_t<uint16_t>();
  number_t<uint32_t>();
  number_t<uint64_t>();
  number_t<size_t>();
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_construct_with_helper) {
  number<double>();
  number<float>();
  number<int8_t>();
  number<int16_t>();
  number<int32_t>();
  number<int64_t>();
  number<uint8_t>();
  number<uint16_t>();
  number<uint32_t>();
  number<uint64_t>();
  number<size_t>();
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_construct_with_default_codec) {
  default_codec<double>();
  default_codec<float>();
  default_codec<int8_t>();
  default_codec<int16_t>();
  default_codec<int32_t>();
  default_codec<int64_t>();
  default_codec<uint8_t>();
  default_codec<uint16_t>();
  default_codec<uint32_t>();
  default_codec<uint64_t>();
  default_codec<size_t>();
}

/*
 * Decoding Floating Point Numbers
 */

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_exactly) {
  BOOST_CHECK_EQUAL(test_decode(number<double>(), "0.5"), 0.5);
  BOOST_CHECK_EQUAL(test_decode(number<float>(), "0.5"), 0.5);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_with_rounding_errors) {
  // 1.1 is mean because it can't be encoded precisely using binary floats
  BOOST_CHECK_EQUAL(test_decode(number<double>(), "1.1"), 1.1);
  BOOST_CHECK_EQUAL(test_decode(number<float>(), "1.1"), 1.1f);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_invalid_float_numbers) {
  test_decode_fail(number<double>(), "");
  test_decode_fail(number<double>(), " 1");
  test_decode_fail(number<double>(), "a");
  test_decode_fail(number<float>(), "NaN");
  test_decode_fail(number<float>(), "- 1.1");
}

/*
 * Encoding Floating Point Numbers
 */

BOOST_AUTO_TEST_CASE(json_codec_number_should_encode_exactly) {
  BOOST_CHECK_EQUAL(encode(0.5), "0.5");
  BOOST_CHECK_EQUAL(encode(0.5f), "0.5");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_encode_not_a_number) {
  BOOST_CHECK_THROW(encode(number<float>(), NAN), encode_exception);
  BOOST_CHECK_THROW(encode(number<double>(), NAN), encode_exception);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_encode_infinity) {
  BOOST_CHECK_THROW(encode(number<float>(), -INFINITY), encode_exception);
  BOOST_CHECK_THROW(encode(number<float>(), +INFINITY), encode_exception);
  BOOST_CHECK_THROW(encode(number<double>(), -INFINITY), encode_exception);
  BOOST_CHECK_THROW(encode(number<double>(), +INFINITY), encode_exception);
}

/*
 * Decoding Signed Integers
 */

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_positive_integer) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "127"), 127);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "32767"), 32767);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "2147483647"), INT32_MAX);
  BOOST_CHECK_EQUAL(test_decode(number<int64_t>(), "9223372036854775807"), INT64_MAX);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_negative_integer) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "-128"), -128);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "-32768"), -32768);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "-2147483648"), INT32_MIN);
  BOOST_CHECK_EQUAL(test_decode(number<int64_t>(), "-9223372036854775808"), INT64_MIN);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_zero_integer_with_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "0e-1"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "0E-1"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "0E+1000000"), 0);  // large exponent
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "0E-1000000"), 0);  // large exponent
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "0E+184467440737095516150"), 0);  // crazy large exponent
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "0E-184467440737095516150"), 0);  // crazy large exponent
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_positive_integer_with_negative_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "1277e-1"), 127);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "327677E-1"), 32767);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "214748364700e-2"), INT32_MAX);
  BOOST_CHECK_EQUAL(test_decode(number<int64_t>(), "922337203685477580700E-2"), INT64_MAX);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_positive_integer_with_positive_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "12e1"), 120);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "3276e+1"), 32760);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "21474836E2"), 2147483600);
  BOOST_CHECK_EQUAL(test_decode(number<int64_t>(), "92233720368547758E+2"), INT64_C(9223372036854775800));
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_negative_integer_with_negative_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "-1288e-1"), -128);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "-327688E-1"), -32768);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "-214748364800e-2"), INT32_MIN);
  BOOST_CHECK_EQUAL(test_decode(number<int64_t>(), "-922337203685477580800E-2"), INT64_MIN);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_negative_integer_with_positive_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "-12e1"), -120);
  BOOST_CHECK_EQUAL(test_decode(number<int16_t>(), "-3276e+1"), -32760);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "-21474836E2"), -2147483600);
  BOOST_CHECK_EQUAL(test_decode(number<int64_t>(), "-92233720368547758E+2"), INT64_C(-9223372036854775800));
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_negative_integer_with_large_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "-1e-1000000"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "-0.1E-1000000"), 0);
  test_decode_fail(number<int16_t>(), "-1E1000000");
  test_decode_fail(number<int64_t>(), "-0.1e+1000000");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_negative_integer_with_overflowing_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "-1e-184467440737095516150"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "-0.1E-184467440737095516150"), 0);
  test_decode_fail(number<int16_t>(), "-1E184467440737095516150");
  test_decode_fail(number<int64_t>(), "-0.1e+184467440737095516150");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_positive_integer_with_large_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "1e-1000000"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "0.1E-1000000"), 0);
  test_decode_fail(number<int16_t>(), "1E1000000");
  test_decode_fail(number<int64_t>(), "0.1e+1000000");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_signed_positive_integer_with_overflowing_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<int8_t>(), "1e-184467440737095516150"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<int32_t>(), "0.1E-184467440737095516150"), 0);
  test_decode_fail(number<int16_t>(), "1E184467440737095516150");
  test_decode_fail(number<int64_t>(), "0.1e+184467440737095516150");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_overflowing_signed_positive_integer) {
  test_decode_fail(number<int8_t>(), "128");
  test_decode_fail(number<int16_t>(), "32768");
  test_decode_fail(number<int32_t>(), "2147483648");
  test_decode_fail(number<int64_t>(), "9223372036854775808");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_underflowing_signed_negative_integer) {
  test_decode_fail(number<int8_t>(), "-129");
  test_decode_fail(number<int16_t>(), "-32769");
  test_decode_fail(number<int32_t>(), "-2147483649");
  test_decode_fail(number<int64_t>(), "-9223372036854775809");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_invalid_signed_integers) {
  test_decode_fail(number<int>(), "");
  test_decode_fail(number<int>(), " 1");
  test_decode_fail(number<int>(), "a");
  test_decode_fail(number<int>(), "NaN");
  test_decode_fail(number<int>(), "- 1.1");
  test_decode_fail(number<int>(), "-1..1");
  test_decode_fail(number<int>(), "-1.e1");
  test_decode_fail(number<int>(), "-1e");
  test_decode_fail(number<int>(), "1e-");
  test_decode_fail(number<int>(), "1e+");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_gobble_characters_after_signed_integer) {
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "-15]", 3), -15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "-15}", 3), -15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "-15,", 3), -15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "-15#", 3), -15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "15.0,", 4), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "15.0]", 4), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "15.0}", 4), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<int8_t>(), "15.0#", 4), 15);
}

/*
 * Encoding Signed Integers
 */

BOOST_AUTO_TEST_CASE(json_codec_number_should_encode_signed_positive_integer) {
  BOOST_CHECK_EQUAL(encode(number<int8_t>(), 0), "0");
  BOOST_CHECK_EQUAL(encode(number<int8_t>(), 127), "127");
  BOOST_CHECK_EQUAL(encode(number<int16_t>(), 32767), "32767");
  BOOST_CHECK_EQUAL(encode(number<int32_t>(), INT32_MAX), "2147483647");
  BOOST_CHECK_EQUAL(encode(number<int64_t>(), INT64_MAX), "9223372036854775807");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_encode_signed_negative_integer) {
  BOOST_CHECK_EQUAL(encode(number<int8_t>(), -128), "-128");
  BOOST_CHECK_EQUAL(encode(number<int16_t>(), -32768), "-32768");
  BOOST_CHECK_EQUAL(encode(number<int32_t>(), INT32_MIN), "-2147483648");
  BOOST_CHECK_EQUAL(encode(number<int64_t>(), INT64_MIN), "-9223372036854775808");
}

/*
 * Decoding Unsigned Integers
 */

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_unsigned_positive_integer) {
  BOOST_CHECK_EQUAL(test_decode(number<uint8_t>(), "255"), 255);
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "65535"), 65535);
  BOOST_CHECK_EQUAL(test_decode(number<uint32_t>(), "4294967295"), 4294967295);
  BOOST_CHECK_EQUAL(test_decode(number<uint64_t>(), "18446744073709551615"), UINT64_MAX);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_unsigned_zero_integer_with_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<uint8_t>(), "0e-1"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "0E-1"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "0E+1000000"), 0);  // large exponent
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "0E-1000000"), 0);  // large exponent
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "0E+184467440737095516150"), 0);  // crazy large exponent
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "0E-184467440737095516150"), 0);  // crazy large exponent
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_unsigned_positive_integer_with_negative_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<uint8_t>(), "2555e-1"), 255);
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "655355E-1"), 65535);
  BOOST_CHECK_EQUAL(test_decode(number<uint32_t>(), "429496729500e-2"), 4294967295);
  BOOST_CHECK_EQUAL(test_decode(number<uint64_t>(), "1844674407370955161500E-2"), UINT64_MAX);
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_unsigned_positive_integer_with_positive_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<uint8_t>(), "25e1"), 250);
  BOOST_CHECK_EQUAL(test_decode(number<uint16_t>(), "6553e+1"), 65530);
  BOOST_CHECK_EQUAL(test_decode(number<uint32_t>(), "42949672E2"), 4294967200);
  BOOST_CHECK_EQUAL(test_decode(number<uint64_t>(), "184467440737095516E+2"), UINT64_C(18446744073709551600));
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_unsigned_positive_integer_with_large_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<uint8_t>(), "1e-1000000"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<uint32_t>(), "0.1E-1000000"), 0);
  test_decode_fail(number<uint16_t>(), "1E1000000");
  test_decode_fail(number<uint64_t>(), "0.1e+1000000");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_decode_unsigned_positive_integer_with_overflowing_exponent) {
  BOOST_CHECK_EQUAL(test_decode(number<uint8_t>(), "1e-184467440737095516150"), 0);
  BOOST_CHECK_EQUAL(test_decode(number<uint32_t>(), "0.1E-184467440737095516150"), 0);
  test_decode_fail(number<uint16_t>(), "1E184467440737095516150");
  test_decode_fail(number<uint64_t>(), "0.1e+184467440737095516150");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_overflowing_unsigned_positive_integer) {
  test_decode_fail(number<uint8_t>(), "256");
  test_decode_fail(number<uint16_t>(), "65536");
  test_decode_fail(number<uint32_t>(), "4294967296");
  test_decode_fail(number<uint64_t>(), "18446744073709551616");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_overflowing_unsigned_positive_integer_with_exponent) {
  test_decode_fail(number<uint8_t>(), "25e2");
  test_decode_fail(number<uint16_t>(), "6553e2");
  test_decode_fail(number<uint32_t>(), "429496729e2");
  test_decode_fail(number<uint64_t>(), "1844674407370955161e2");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_decode_invalid_unsigned_integers) {
  test_decode_fail(number<unsigned>(), "-10");  // unsigned must never be negative
  test_decode_fail(number<unsigned>(), "");
  test_decode_fail(number<unsigned>(), " 1");
  test_decode_fail(number<unsigned>(), "a");
  test_decode_fail(number<unsigned>(), "NaN");
  test_decode_fail(number<unsigned>(), "- 1.1");
  test_decode_fail(number<unsigned>(), "1..1");
  test_decode_fail(number<unsigned>(), "1.e1");
  test_decode_fail(number<unsigned>(), "1e");
  test_decode_fail(number<unsigned>(), "1e-");
  test_decode_fail(number<unsigned>(), "1e+");
}

BOOST_AUTO_TEST_CASE(json_codec_number_should_not_gobble_characters_after_unsigned_integer) {
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15]", 2), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15}", 2), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15,", 2), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15#", 2), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15.0,", 4), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15.0]", 4), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15.0}", 4), 15);
  BOOST_CHECK_EQUAL(test_decode_dont_gobble(number<uint8_t>(), "15.0#", 4), 15);
}

/*
 * Encoding Unsigned Integers
 */

BOOST_AUTO_TEST_CASE(json_codec_number_should_encode_unsigned_positive_integer) {
  BOOST_CHECK_EQUAL(encode(number<uint8_t>(), 0), "0");
  BOOST_CHECK_EQUAL(encode(number<uint8_t>(), 255), "255");
  BOOST_CHECK_EQUAL(encode(number<uint16_t>(), 65535), "65535");
  BOOST_CHECK_EQUAL(encode(number<uint32_t>(), 4294967295), "4294967295");
  BOOST_CHECK_EQUAL(encode(number<uint64_t>(), UINT64_MAX), "18446744073709551615");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
