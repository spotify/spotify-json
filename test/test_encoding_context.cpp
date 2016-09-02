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

#include <spotify/json/encoding_context.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

BOOST_AUTO_TEST_CASE(json_encoding_context_should_construct_without_capacity) {
  const encoding_context ctx;
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_NE(ctx.capacity(), 0);
  BOOST_CHECK(ctx.empty());
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_construct_with_capacity) {
  const encoding_context ctx(1234);
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_EQUAL(ctx.capacity(), 1234);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_construct_with_no_capacity) {
  const encoding_context ctx(0);
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_EQUAL(ctx.capacity(), 0);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_reserve_bytes) {
  encoding_context ctx(0);
  BOOST_CHECK(ctx.reserve(1234));
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_GE(ctx.capacity(), 1234);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_return_same_address_for_multiple_reservations) {
  encoding_context ctx(0);
  const auto address_0 = ctx.reserve(1);
  const auto address_1 = ctx.reserve(9);
  const auto address_2 = ctx.reserve(5);
  BOOST_CHECK(address_0 == address_1);
  BOOST_CHECK(address_0 == address_2);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_advance_pointer_after_reservation) {
  encoding_context ctx(0);
  ctx.advance(0); BOOST_CHECK(ctx.reserve(1024) == &ctx.data()[0]);
  ctx.advance(1); BOOST_CHECK(ctx.reserve(1024) == &ctx.data()[1]);
  ctx.advance(2); BOOST_CHECK(ctx.reserve(1024) == &ctx.data()[3]);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_maintain_correct_size_when_advancing) {
  encoding_context ctx(0);
  ctx.advance(1);
  ctx.advance(1);
  ctx.advance(2);
  ctx.advance(3);
  ctx.advance(5);
  BOOST_CHECK_EQUAL(ctx.size(), 12);
  BOOST_CHECK(!ctx.empty());
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_append_single_byte) {
  encoding_context ctx;
  ctx.append('1');
  ctx.append('2');
  BOOST_CHECK_EQUAL(ctx.data()[0], '1');
  BOOST_CHECK_EQUAL(ctx.data()[1], '2');
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_replace_last_byte) {
  encoding_context ctx;
  ctx.append('1');
  ctx.append_or_replace('1', '2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 1);
  BOOST_CHECK_EQUAL(ctx.data()[0], '2');
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_not_replace_wrong_last_byte) {
  encoding_context ctx;
  ctx.append('1');
  ctx.append_or_replace('3', '2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 2);
  BOOST_CHECK_EQUAL(ctx.data()[0], '1');
  BOOST_CHECK_EQUAL(ctx.data()[1], '2');
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_not_replace_in_empty_context) {
  encoding_context ctx;
  ctx.append_or_replace('1', '2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 1);
  BOOST_CHECK_EQUAL(ctx.data()[0], '2');
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_append_multiple_bytes) {
  encoding_context ctx;
  ctx.append("12", 3);
  BOOST_CHECK_EQUAL(ctx.data()[0], '1');
  BOOST_CHECK_EQUAL(ctx.data()[1], '2');
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_throw_exception_on_small_size_overflow) {
  detail::base_encoding_context<uint16_t> ctx(0);
  ctx.reserve(UINT16_MAX);
  ctx.advance(UINT16_MAX);
  BOOST_CHECK_EQUAL(ctx.size(), UINT16_MAX);
  BOOST_CHECK_EQUAL(ctx.capacity(), UINT16_MAX);
  BOOST_CHECK_THROW(ctx.reserve(1), std::bad_alloc);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_throw_exception_on_large_size_overflow) {
  detail::base_encoding_context<uint16_t> ctx(0);
  ctx.reserve(UINT16_MAX);
  ctx.advance(UINT16_MAX);
  BOOST_CHECK_EQUAL(ctx.size(), UINT16_MAX);
  BOOST_CHECK_EQUAL(ctx.capacity(), UINT16_MAX);
  BOOST_CHECK_THROW(ctx.reserve(UINT16_MAX), std::bad_alloc);
}

BOOST_AUTO_TEST_CASE(json_encoding_context_should_saturate_capacity_on_overflow) {
  detail::base_encoding_context<uint16_t> ctx(UINT16_MAX - 20);
  ctx.reserve(UINT16_MAX - 10);
  BOOST_CHECK_EQUAL(ctx.capacity(), UINT16_MAX);
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
