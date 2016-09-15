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

#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/bitset.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

BOOST_AUTO_TEST_CASE(bitset_should_test_and_set_in_single_index_bitset) {
  bitset<1> bs(1);
  BOOST_CHECK_EQUAL(bs.test_and_set(0), 0);
  BOOST_CHECK_EQUAL(bs.test_and_set(0), 1);
  BOOST_CHECK_EQUAL(bs.test_and_set(0), 1);
}

BOOST_AUTO_TEST_CASE(bitset_should_test_and_set_within_inline_size) {
  bitset<32> bs(32);
  for (std::size_t i = 0; i < 32; i++) {
    BOOST_CHECK_EQUAL(bs.test_and_set(i), 0);
    BOOST_CHECK_EQUAL(bs.test_and_set(i), 1);
    BOOST_CHECK_EQUAL(bs.test_and_set(i), 1);
  }
}

BOOST_AUTO_TEST_CASE(bitset_should_test_and_set_outside_inline_size) {
  bitset<32> bs(1024 * 1024);
  for (std::size_t i = 0; i < 1024 * 1024; i++) {
    BOOST_CHECK_EQUAL(bs.test_and_set(i), 0);
    BOOST_CHECK_EQUAL(bs.test_and_set(i), 1);
    BOOST_CHECK_EQUAL(bs.test_and_set(i), 1);
  }
}

BOOST_AUTO_TEST_CASE(bitset_should_not_overrun_bitset_buffer) {
  struct {
    bitset<8> bs = { 8 };
    uint32_t canary = 0;
  } s;

  s.bs.test_and_set(7);
  BOOST_CHECK_EQUAL(s.canary, 0);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
