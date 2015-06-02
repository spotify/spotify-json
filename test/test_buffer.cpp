/*
 * Copyright (c) 2014 Spotify AB
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

#include <inttypes.h>
#include <limits>
#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/buffer.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

std::string to_string(const buffer &buffer) {
  return std::string(buffer.data(), buffer.size());
}

const void *to_void(const char *ptr) {
  return static_cast<const void *>(ptr);
}


}  // anonymous namespace

BOOST_AUTO_TEST_CASE(buffer_with_zero_capacity_should_grow) {
  buffer buffer(0);
  BOOST_CHECK_EQUAL(0u, buffer.capacity());
  buffer.put(0);
  BOOST_CHECK_GT(buffer.capacity(), 0u);
}

BOOST_AUTO_TEST_CASE(buffer_initial_capacity_should_be_correct) {
  buffer buffer(1024);
  BOOST_CHECK_EQUAL(1024, buffer.capacity());
}

BOOST_AUTO_TEST_CASE(buffer_size_should_be_correct) {
  buffer buffer;
  BOOST_CHECK_EQUAL(0u, buffer.size());
  buffer.put(0);
  BOOST_CHECK_EQUAL(1u, buffer.size());
}

BOOST_AUTO_TEST_CASE(buffer_should_be_copy_constructible) {
  const char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

  json::buffer buffer1;
  buffer1.write(data, sizeof(data));

  json::buffer buffer2(buffer1);

  BOOST_CHECK_NE(to_void(buffer1.data()), to_void(buffer2.data()));
  BOOST_CHECK_EQUAL(buffer1.size(), buffer2.size());
  BOOST_CHECK_EQUAL(buffer1.capacity(), buffer2.capacity());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      buffer1.begin(), buffer1.end(),
      buffer2.begin(), buffer2.end());
}

BOOST_AUTO_TEST_CASE(buffer_should_be_assignable) {
  const char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

  json::buffer buffer1;
  json::buffer buffer2;
  buffer1.write(data, sizeof(data));

  buffer2 = buffer1;

  BOOST_CHECK_NE(to_void(buffer1.data()), to_void(buffer2.data()));
  BOOST_CHECK_EQUAL(buffer1.size(), buffer2.size());
  BOOST_CHECK_EQUAL(buffer1.capacity(), buffer2.capacity());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      buffer1.begin(), buffer1.end(),
      buffer2.begin(), buffer2.end());
}

BOOST_AUTO_TEST_CASE(buffer_write_should_write_correct_bytes) {
  const char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
  buffer buffer;
  buffer.write(data, sizeof(data));
  BOOST_CHECK_EQUAL_COLLECTIONS(
      data, data + sizeof(data),
      buffer.data(), buffer.data() + buffer.size());
}

BOOST_AUTO_TEST_CASE(buffer_put_should_write_correct_byte) {
  buffer buffer;
  buffer.put(0x01);
  BOOST_REQUIRE_EQUAL(1u, buffer.size());
  BOOST_CHECK_EQUAL(0x01, buffer.data()[0]);
}

BOOST_AUTO_TEST_CASE(buffer_stream_operator_should_write_correct_char) {
  buffer buffer;
  buffer << 'x';
  BOOST_REQUIRE_EQUAL(1u, buffer.size());
  BOOST_CHECK_EQUAL('x', buffer.data()[0]);
}

BOOST_AUTO_TEST_CASE(buffer_stream_operator_should_write_correct_string) {
  std::string data("2a0cb6b42add7808a6ab3619930fa7983c9ce7fe");
  buffer buffer;
  buffer << data;
  BOOST_CHECK_EQUAL_COLLECTIONS(
      data.begin(), data.end(),
      buffer.data(), buffer.data() + buffer.size());
}

BOOST_AUTO_TEST_CASE(buffer_check_float_with_imprecise_representation) {
  buffer buffer;
  buffer << 1.1f;
  const auto str = to_string(buffer);
  BOOST_REQUIRE(str.size() >= 3);
  BOOST_CHECK_EQUAL("1.1", str.substr(0, 3));
}

BOOST_AUTO_TEST_CASE(buffer_check_negative_int8_t) {
  buffer buffer;
  buffer << std::numeric_limits<int8_t>::min();
  BOOST_CHECK_EQUAL("-128", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_positive_int8_t) {
  buffer buffer;
  buffer << std::numeric_limits<int8_t>::max();
  BOOST_CHECK_EQUAL("127", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_negative_int16_t) {
  buffer buffer;
  buffer << std::numeric_limits<int16_t>::min();
  BOOST_CHECK_EQUAL("-32768", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_positive_int16_t) {
  buffer buffer;
  buffer << std::numeric_limits<int16_t>::max();
  BOOST_CHECK_EQUAL("32767", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_negative_int32_t) {
  buffer buffer;
  buffer << std::numeric_limits<int32_t>::min();
  BOOST_CHECK_EQUAL("-2147483648", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_positive_int32_t) {
  buffer buffer;
  buffer << std::numeric_limits<int32_t>::max();
  BOOST_CHECK_EQUAL("2147483647", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_negative_int64_t) {
  buffer buffer;
  buffer << std::numeric_limits<int64_t>::min();
  BOOST_CHECK_EQUAL("-9223372036854775808", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_positive_int64_t) {
  buffer buffer;
  buffer << std::numeric_limits<int64_t>::max();
  BOOST_CHECK_EQUAL("9223372036854775807", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_uint8_t) {
  buffer buffer;
  buffer << std::numeric_limits<uint8_t>::max();
  BOOST_CHECK_EQUAL("255", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_uint16_t) {
  buffer buffer;
  buffer << std::numeric_limits<uint16_t>::max();
  BOOST_CHECK_EQUAL("65535", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_uint32_t) {
  buffer buffer;
  buffer << std::numeric_limits<uint32_t>::max();
  BOOST_CHECK_EQUAL("4294967295", to_string(buffer));
}

BOOST_AUTO_TEST_CASE(buffer_check_uint64_t) {
  buffer buffer;
  buffer << std::numeric_limits<uint64_t>::max();
  BOOST_CHECK_EQUAL("18446744073709551615", to_string(buffer));
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
