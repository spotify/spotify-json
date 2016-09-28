/*
 * Copyright (c) 2016 Spotify AB
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

#include <cstdlib>
#include <limits>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/encode_integer.hpp>
#include <spotify/json/encode_context.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

template <typename T>
void verify_encode_one_negative(encode_context &context, T value) {
  encode_negative_integer(context, value);
  context.append(0);  // null terminator for std::strtoll
  const auto begin = static_cast<const char *>(context.data());
  const auto encoded_value = std::strtoll(begin, nullptr, 10);
  BOOST_REQUIRE_EQUAL(value, encoded_value);
  context.clear();
}

template <typename T>
void verify_encode_one_positive(encode_context &context, T value) {
  encode_positive_integer(context, value);
  context.append(0);  // null terminator for std::strtoull
  const auto begin = static_cast<const char *>(context.data());
  const auto encoded_value = std::strtoull(begin, nullptr, 10);
  BOOST_REQUIRE_EQUAL(value, encoded_value);
  context.clear();
}

template <typename T>
void verify_encode_all_negative(const T stride = 1) {
  encode_context context;
  constexpr int64_t min = std::numeric_limits<T>::min();
  const auto steps = -(min / stride);
  for (int64_t i = 0; i <= steps; i++) {
    verify_encode_one_negative(context, -i * stride);
  }
  // Verify the 'min' value as well, for the cases where the 'stride' misses it.
  verify_encode_one_negative(context, T(min));
}

template <typename T>
void verify_encode_all_positive(const T stride = 1) {
  encode_context context;
  constexpr uint64_t max = std::numeric_limits<T>::max();
  const auto steps = max / stride;
  for (uint64_t i = 0; i <= steps; i++) {
    verify_encode_one_positive(context, i * stride);
  }
  // Verify the 'max' value as well, for the cases where the 'stride' misses it.
  verify_encode_one_positive(context, T(max));
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_encode_integer_int8_t) {
  verify_encode_all_negative<int8_t>();
  verify_encode_all_positive<int8_t>();
}

BOOST_AUTO_TEST_CASE(json_encode_integer_uint8_t) {
  verify_encode_all_positive<uint8_t>();
}

BOOST_AUTO_TEST_CASE(json_encode_integer_int16_t) {
  verify_encode_all_negative<int16_t>();
  verify_encode_all_positive<int16_t>();
}

BOOST_AUTO_TEST_CASE(json_encode_integer_uint16_t) {
  verify_encode_all_positive<uint16_t>();
}

BOOST_AUTO_TEST_CASE(json_encode_integer_int32_t) {
  constexpr auto stride = 17001;
  verify_encode_all_negative<int32_t>(stride);
  verify_encode_all_positive<int32_t>(stride);
}

BOOST_AUTO_TEST_CASE(json_encode_integer_uint32_t) {
  constexpr auto stride = 17001;
  verify_encode_all_positive<uint32_t>(stride);
}

BOOST_AUTO_TEST_CASE(json_encode_integer_int64_t) {
  constexpr auto stride = 69080706050301LL;
  verify_encode_all_negative<int64_t>(stride);
  verify_encode_all_positive<int64_t>(stride);
}

BOOST_AUTO_TEST_CASE(json_encode_integer_uint64_t) {
  constexpr auto stride = 69080706050301ULL;
  verify_encode_all_positive<uint64_t>(stride);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
