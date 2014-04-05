// Copyright (c) 2014 Felix Bruns.

#include <boost/test/unit_test.hpp>

#include <string>

#include "json_key.hpp"

BOOST_AUTO_TEST_SUITE(json)

using namespace std;

namespace {

string to_string(const json::key &key) {
  return string(key.data, key.size);
}

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(json_key_should_quote_raw_key) {
  BOOST_CHECK_EQUAL("\"i_am_a_key\"", to_string(json::key("i_am_a_key")));
}

BOOST_AUTO_TEST_CASE(json_key_should_escape_raw_key) {
  BOOST_CHECK_EQUAL("\"\\u0000\\\"\"", to_string(json::key(string("\0\"", 2))));
}

BOOST_AUTO_TEST_SUITE_END()  // json
