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

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/encode_context.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

BOOST_AUTO_TEST_CASE(json_encode_context_should_construct_without_capacity) {
  const encode_context ctx;
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_NE(ctx.capacity(), 0);
  BOOST_CHECK(ctx.empty());
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_construct_with_capacity) {
  const encode_context ctx(1234);
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_EQUAL(ctx.capacity(), 1234);
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_construct_with_no_capacity) {
  const encode_context ctx(0);
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_EQUAL(ctx.capacity(), 0);
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_reserve_bytes) {
  encode_context ctx(0);
  BOOST_CHECK(ctx.reserve(1234));
  BOOST_CHECK_EQUAL(ctx.size(), 0);
  BOOST_CHECK_GE(ctx.capacity(), 1234);
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_return_same_address_for_multiple_reservations) {
  encode_context ctx(0);
  const auto address_0 = ctx.reserve(9);
  const auto address_1 = ctx.reserve(5);
  const auto address_2 = ctx.reserve(1);
  BOOST_CHECK(address_0 == address_1);
  BOOST_CHECK(address_0 == address_2);
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_advance_pointer_after_reservation) {
  encode_context ctx(0);
  ctx.advance(0); BOOST_CHECK(ctx.reserve(1024) == &ctx.data()[0]);
  ctx.advance(1); BOOST_CHECK(ctx.reserve(1024) == &ctx.data()[1]);
  ctx.advance(2); BOOST_CHECK(ctx.reserve(1024) == &ctx.data()[3]);
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_maintain_correct_size_when_advancing) {
  encode_context ctx(0);
  ctx.advance(1);
  ctx.advance(1);
  ctx.advance(2);
  ctx.advance(3);
  ctx.advance(5);
  BOOST_CHECK_EQUAL(ctx.size(), 12);
  BOOST_CHECK(!ctx.empty());
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_append_single_byte) {
  encode_context ctx;
  ctx.append('1');
  ctx.append('2');
  BOOST_CHECK_EQUAL(ctx.data()[0], '1');
  BOOST_CHECK_EQUAL(ctx.data()[1], '2');
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_replace_last_byte) {
  encode_context ctx;
  ctx.append('1');
  ctx.append_or_replace('1', '2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 1);
  BOOST_CHECK_EQUAL(ctx.data()[0], '2');
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_not_replace_wrong_last_byte) {
  encode_context ctx;
  ctx.append('1');
  ctx.append_or_replace('3', '2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 2);
  BOOST_CHECK_EQUAL(ctx.data()[0], '1');
  BOOST_CHECK_EQUAL(ctx.data()[1], '2');
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_not_replace_in_empty_context) {
  encode_context ctx;
  ctx.append_or_replace('1', '2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 1);
  BOOST_CHECK_EQUAL(ctx.data()[0], '2');
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_append_multiple_bytes) {
  encode_context ctx;
  ctx.append("12", 3);
  BOOST_CHECK_EQUAL(ctx.data()[0], '1');
  BOOST_CHECK_EQUAL(ctx.data()[1], '2');
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_let_data_be_stolen) {
  encode_context ctx;
  ctx.append('1');
  const auto stolen_data = ctx.steal_data();
  BOOST_CHECK_EQUAL(static_cast<char *>(stolen_data.get())[0], '1');
}

BOOST_AUTO_TEST_CASE(json_encode_context_should_reset_when_data_is_stolen) {
  encode_context ctx;
  ctx.append('1');
  const auto stolen_data = ctx.steal_data();
  BOOST_REQUIRE(ctx.empty());
  ctx.append('2');
  BOOST_REQUIRE_EQUAL(ctx.size(), 1);
  BOOST_CHECK_EQUAL(ctx.data()[0], '2');
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
