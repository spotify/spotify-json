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

#include <boost/test/unit_test.hpp>

#include <string>

#include "json_pair.hpp"

BOOST_AUTO_TEST_SUITE(json)

using namespace std;

BOOST_AUTO_TEST_CASE(json_pair_should_reference_key_and_value_string) {
  typedef json::pair<string, string> string_pair;

  string key("key");
  string value("value");
  string_pair pair(json::make_pair(key, value));

  BOOST_CHECK_EQUAL("key", pair.key);
  BOOST_CHECK_EQUAL("value", pair.value);

  key = "foo";
  value = "bar";

  BOOST_CHECK_EQUAL("foo", pair.key);
  BOOST_CHECK_EQUAL("bar", pair.value);
}

BOOST_AUTO_TEST_CASE(json_pair_should_reference_key_and_value_cstr) {
  typedef const char * cstr;
  typedef json::pair<cstr, cstr> cstr_pair;

  cstr key = "key";
  cstr value = "value";
  cstr_pair pair(json::make_pair(key, value));
  
  BOOST_CHECK_EQUAL(key, pair.key);
  BOOST_CHECK_EQUAL(value, pair.value);
}

BOOST_AUTO_TEST_SUITE_END()  // json
