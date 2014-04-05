// Copyright (c) 2014 Felix Bruns.

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
