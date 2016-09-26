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

#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/skip_value.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

void verify_skip_fail(const std::string &json) {
  auto context = decode_context(json.data(), json.data() + json.size());
  BOOST_CHECK_THROW(skip_value(context), decode_exception);
}

void verify_skip_value(const std::string &json, const size_t extra = 0) {
  auto context = decode_context(json.data(), json.data() + json.size());
  const auto original_context = context;
  skip_value(context);
  BOOST_CHECK_EQUAL(context.position, original_context.end - extra);
  BOOST_CHECK_EQUAL(context.end, original_context.end);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_skip_value_string) {
  verify_skip_value("\"\"");
  verify_skip_value("\"abc\"");
  verify_skip_value(u8"\"\u9E21\"");
}

BOOST_AUTO_TEST_CASE(json_skip_value_number) {
  verify_skip_value("0");
  verify_skip_value("1");
  verify_skip_value("-0");
  verify_skip_value("-1");
  verify_skip_value("0.5");
  verify_skip_value("1.5");
  verify_skip_value("1e1");
  verify_skip_value("1E1");
  verify_skip_value("1e+1");
  verify_skip_value("1E+1");
  verify_skip_value("1e-1");
  verify_skip_value("1E-1");
  verify_skip_value("-1.3e+2");
  verify_skip_value("-3.1E-2");
  verify_skip_value("123456789123456789123456789000");

  verify_skip_value("01", 1);  // skip "0", leave "1"
  verify_skip_value("1e1.1", 2);  // skip "1e1", leave ".1"
}

BOOST_AUTO_TEST_CASE(json_skip_value_boolean) {
  verify_skip_value("true");
  verify_skip_value("false");
}

BOOST_AUTO_TEST_CASE(json_skip_value_null) {
  verify_skip_value("null");
}

BOOST_AUTO_TEST_CASE(json_skip_value_array) {
  verify_skip_value("[]");
  verify_skip_value("[1,null,true]");
  verify_skip_value("[ 1 , null , true ]");
}

BOOST_AUTO_TEST_CASE(json_skip_value_object) {
  verify_skip_value("{}");
  verify_skip_value(R"({"a":3})");
  verify_skip_value(R"({"a":3,"b":4})");
  verify_skip_value(R"({ "a" : 3 , "b" : 4 })");}

BOOST_AUTO_TEST_CASE(json_skip_value_nested_array) {
  verify_skip_value("[{},[1],[[1]]]");
  verify_skip_value("[1,[1],[[1]]]");
}

BOOST_AUTO_TEST_CASE(json_skip_value_nested_object) {
  verify_skip_value(R"({"a":{}})");
  verify_skip_value(R"({"a":[]})");
  verify_skip_value(R"({"a":[{},[]]})");
}

/*
 * Invalid JSON
 */

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_empty) {
  verify_skip_fail("");
}

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_invalid_character) {
  verify_skip_fail("a");
}

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_invalid_string) {
  verify_skip_fail("\"");
  verify_skip_fail(R"("\a")");
}

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_invalid_number) {
  verify_skip_fail("-");
  verify_skip_fail(".");
  verify_skip_fail(".1");
  verify_skip_fail("1.");
  verify_skip_fail("1..1");
  verify_skip_fail("e");
  verify_skip_fail("E");
  verify_skip_fail("1e");
  verify_skip_fail("1E");
}

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_invalid_boolean) {
  verify_skip_fail("tru");
  verify_skip_fail("FALSE");
}

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_invalid_object) {
  verify_skip_fail(R"({true:false})");
  verify_skip_fail(R"({"true":false)");
}

BOOST_AUTO_TEST_CASE(json_skip_value_should_not_skip_invalid_array) {
  verify_skip_fail("[,]");
  verify_skip_fail("[12");
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
