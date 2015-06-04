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

#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/key.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

using namespace std;

namespace {

string to_string(const json::key &key) {
  return string(key.data, key.size);
}

key make_copied_key(const char *str) {
  key a(str);
  const key b(a);
  return b;
}

key make_move_constructed_key(const char *str) {
  key a(str);
  const key b(std::move(a));
  return b;
}

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(json_key_should_quote_raw_key) {
  BOOST_CHECK_EQUAL("\"i_am_a_key\"", to_string(json::key("i_am_a_key")));
}

BOOST_AUTO_TEST_CASE(json_key_should_quote_string_key) {
  BOOST_CHECK_EQUAL(std::string("\"i_am_a_key\""), to_string(json::key("i_am_a_key")));
}

BOOST_AUTO_TEST_CASE(json_key_should_quote_raw_key_with_size) {
  BOOST_CHECK_EQUAL("\"i_a\"", to_string(json::key("i_am_a_key", 3)));
}

BOOST_AUTO_TEST_CASE(json_key_should_escape_raw_key) {
  BOOST_CHECK_EQUAL("\"\\u0000\\\"\"", to_string(json::key(string("\0\"", 2))));
}

BOOST_AUTO_TEST_CASE(json_key_should_copy_correctly) {
  const key k(make_copied_key("abcdef"));
  BOOST_CHECK_EQUAL("\"abcdef\"", to_string(k));
}

BOOST_AUTO_TEST_CASE(json_key_should_move_construct_correctly) {
  const key k(make_move_constructed_key("abcdef"));
  BOOST_CHECK_EQUAL("\"abcdef\"", to_string(k));
}

BOOST_AUTO_TEST_CASE(json_key_should_assign_correctly) {
  key a("abcdef");
  const key b(a);
  a = json::key("123456");

  BOOST_CHECK_EQUAL("\"abcdef\"", to_string(b));
}

BOOST_AUTO_TEST_CASE(json_key_should_move_assign_correctly) {
  key a("abcdef");
  key b("123456");
  a = std::move(b);

  BOOST_CHECK_EQUAL("\"123456\"", to_string(a));
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
