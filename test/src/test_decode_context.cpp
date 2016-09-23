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

#include <spotify/json/decode_context.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

BOOST_AUTO_TEST_CASE(json_decode_context_should_construct_with_begin_end) {
  static const char string[] = "abc";
  const char * const end = string + sizeof(string);
  const decode_context ctx(string, end);

  BOOST_CHECK_EQUAL(ctx.begin, string);
  BOOST_CHECK_EQUAL(ctx.position, string);
  BOOST_CHECK_EQUAL(ctx.end, end);
}

BOOST_AUTO_TEST_CASE(json_decode_context_should_construct_with_data_size) {
  static const char string[] = "abc";
  const char * const end = string + sizeof(string);
  const decode_context ctx(string, sizeof(string));

  BOOST_CHECK_EQUAL(ctx.begin, string);
  BOOST_CHECK_EQUAL(ctx.position, string);
  BOOST_CHECK_EQUAL(ctx.end, end);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
