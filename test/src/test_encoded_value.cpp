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

#include <cstring>
#include <limits>
#include <sstream>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

#include <spotify/json/decode_exception.hpp>
#include <spotify/json/encoded_value.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

std::string value_to_string(const encoded_value_ref &value_ref) {
  return std::string(value_ref.data(), value_ref.size());
}

using encoded_value_types = boost::mpl::list<encoded_value_ref, encoded_value>;

}  // namespace

/*
 * json::encoded_value_ref & json::encoded_value
 */

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_default_construct,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type value;
  BOOST_CHECK_EQUAL(value_to_string(value), "null");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_validate_given_json_for_cstr,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type("[ null, 1234 ]");
  BOOST_CHECK_THROW(encoded_value_type("{ null, 1234 }"), decode_exception);
  BOOST_CHECK_THROW(encoded_value_type("[ null, 123 ] "), decode_exception);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_validate_given_json_for_data_size,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type("[ null, 1234 ]", 14);
  BOOST_CHECK_THROW(encoded_value_type("{ null, 1234 }", 14), decode_exception);
  BOOST_CHECK_THROW(encoded_value_type("[ null, 123 ] ", 14), decode_exception);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_validate_given_json_for_std_string,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type(std::string("[ null, 1234 ]"));
  BOOST_CHECK_THROW(encoded_value_type(std::string("{ null, 1234 }")), decode_exception);
  BOOST_CHECK_THROW(encoded_value_type(std::string("[ null, 123 ] ")), decode_exception);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_not_validate_unsafe_unchecked,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type("nil", encoded_value::unsafe_unchecked());
  encoded_value_type("nil", 3, encoded_value::unsafe_unchecked());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_write_to_ostream,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type value("1234");
  std::stringstream ss;
  ss << "[" << value << "]";
  BOOST_CHECK_EQUAL(ss.str(), "[1234]");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_swap,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type a("nil", encoded_value::unsafe_unchecked());
  encoded_value_type b("nul", encoded_value::unsafe_unchecked());
  a.swap(b);
  BOOST_CHECK_EQUAL(value_to_string(a), "nul");
  BOOST_CHECK_EQUAL(value_to_string(b), "nil");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_compare_for_equality,
    encoded_value_type,
    encoded_value_types) {
  encoded_value_type a("null");
  encoded_value_type b("1");
  BOOST_CHECK(a == a);
  BOOST_CHECK(!(a == b));
  BOOST_CHECK(!(a != a));
  BOOST_CHECK(a != b);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_compare_for_equality_not_pointer_equality,
    encoded_value_type,
    encoded_value_types) {
  char buf_1[] = { '1', '\0' };
  char buf_2[] = { '1', '\0' };
  encoded_value_type a(static_cast<const char *>(buf_1));
  encoded_value_type b(static_cast<const char *>(buf_2));
  BOOST_CHECK(a == b);
  BOOST_CHECK(!(a != b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(
    json_encoded_value_should_compare_with_other_type,
    encoded_value_type,
    encoded_value_types) {
  using other_value_type = typename std::conditional<
      std::is_same<encoded_value_type, encoded_value>::value,
      encoded_value_ref,
      encoded_value>::type;
  encoded_value_type a("null");
  other_value_type b("1");
  BOOST_CHECK(a == a);
  BOOST_CHECK(!(a == b));
  BOOST_CHECK(!(a != a));
  BOOST_CHECK(a != b);
}

/*
 * json::encoded_value_ref
 */

BOOST_AUTO_TEST_CASE(json_encoded_value_ref_should_copy_construct) {
  const auto a = encoded_value_ref{"1234"};
  const auto b = encoded_value_ref{a};
  BOOST_CHECK(a.data() == b.data());
  BOOST_CHECK(a.size() == b.size());
}

BOOST_AUTO_TEST_CASE(json_encoded_value_ref_should_move_construct) {
  auto a = encoded_value_ref{"1234"};
  const auto b = encoded_value_ref{std::move(a)};
  BOOST_CHECK(a.data() == b.data());
  BOOST_CHECK(a.size() == b.size());
}

BOOST_AUTO_TEST_CASE(json_encoded_value_ref_should_assign_from_encoded_value_ref) {
  encoded_value_ref a("nil", encoded_value_ref::unsafe_unchecked());
  encoded_value_ref b;
  encoded_value_ref c;
  b = a;
  c = std::move(a);
  BOOST_CHECK_EQUAL(value_to_string(a), "nil");
  BOOST_CHECK_EQUAL(value_to_string(b), "nil");
  BOOST_CHECK_EQUAL(value_to_string(c), "nil");
}

BOOST_AUTO_TEST_CASE(json_encoded_value_ref_should_assign_from_encoded_value) {
  encoded_value a("nil", encoded_value::unsafe_unchecked());
  encoded_value_ref b;
  encoded_value_ref c;
  b = a;
  c = std::move(a);
  BOOST_CHECK_EQUAL(value_to_string(a), "nil");
  BOOST_CHECK_EQUAL(value_to_string(b), "nil");
  BOOST_CHECK_EQUAL(value_to_string(c), "nil");
}

/*
 * json::encoded_value
 */

BOOST_AUTO_TEST_CASE(json_encoded_value_should_copy_construct) {
  const auto a = encoded_value{"1234"};
  const auto b = encoded_value{a};
  BOOST_CHECK_EQUAL(value_to_string(a), "1234");
  BOOST_CHECK_EQUAL(value_to_string(b), "1234");
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_move_construct) {
  auto a = encoded_value{"1234"};
  const auto b = encoded_value{std::move(a)};
  BOOST_CHECK_EQUAL(value_to_string(a), "null");
  BOOST_CHECK_EQUAL(value_to_string(b), "1234");
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_move_construct_with_context) {
  encode_context context;
  context.append("{}", 2);
  const auto value = encoded_value{std::move(context)};
  BOOST_CHECK_EQUAL(value_to_string(value), "{}");
  BOOST_CHECK_EQUAL(context.size(), 0);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_move_construct_with_context_unchecked) {
  encode_context context;
  context.append("{}", 2);
  const auto value = encoded_value{std::move(context), encoded_value::unsafe_unchecked()};
  BOOST_CHECK_EQUAL(value_to_string(value), "{}");
  BOOST_CHECK_EQUAL(context.size(), 0);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_assign_from_encoded_value_ref) {
  encoded_value_ref a("nil", encoded_value_ref::unsafe_unchecked());
  encoded_value b;
  encoded_value c;
  b = a;
  c = std::move(a);
  BOOST_CHECK_EQUAL(value_to_string(a), "nil");
  BOOST_CHECK_EQUAL(value_to_string(b), "nil");
  BOOST_CHECK_EQUAL(value_to_string(c), "nil");
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_assign_from_encoded_value) {
  encoded_value a("nil", encoded_value::unsafe_unchecked());
  encoded_value b;
  encoded_value c;
  b = a;
  c = std::move(a);
  BOOST_CHECK_EQUAL(value_to_string(a), "null");
  BOOST_CHECK_EQUAL(value_to_string(b), "nil");
  BOOST_CHECK_EQUAL(value_to_string(c), "nil");
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_throw_bad_alloc) {
  const auto way_too_large = std::numeric_limits<size_t>::max();
  BOOST_CHECK_THROW(encoded_value("", way_too_large), std::bad_alloc);
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
