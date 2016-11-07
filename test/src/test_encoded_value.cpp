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
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/decode_exception.hpp>
#include <spotify/json/encoded_value.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

std::vector<char> string_to_vector(const std::string &string) {
  return std::vector<char>(string.begin(), string.end());
}

ref string_to_ref(const char *string_in_memory) {
  return ref(string_in_memory, string_in_memory + std::strlen(string_in_memory));
}

encode_context string_to_context(const std::string &string) {
  encode_context context;
  context.append(string.data(), string.size());
  return context;
}

void take_rvalue_string(std::string &&string) {
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_encoded_value_should_default_construct_string) {
  encoded_value<> value;
  BOOST_CHECK_EQUAL(std::string(value), "null");
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_default_construct_vector) {
  encoded_value<std::vector<char>> value;
  BOOST_CHECK(std::vector<char>(value) == string_to_vector("null"));
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_default_construct_ref) {
  encoded_value<ref> value;
  BOOST_CHECK(!memcmp(ref(value).data(), "null", 4));
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_validate_string) {
  encoded_value<>("[ null, 1234 ]");
  BOOST_CHECK_THROW(encoded_value<>("{ null, 1234 }"), decode_exception);
  BOOST_CHECK_THROW(encoded_value<>("[ null, 123 ] "), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_validate_vector) {
  using vector_value = encoded_value<std::vector<char>>;
  vector_value(string_to_vector("[ null, 1234 ]"));
  BOOST_CHECK_THROW(vector_value(string_to_vector("{ null, 1234 }")), decode_exception);
  BOOST_CHECK_THROW(vector_value(string_to_vector("[ null, 123 ] ")), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_validate_ref) {
  encoded_value<ref>(string_to_ref("[ null, 1234 ]"));
  BOOST_CHECK_THROW(encoded_value<ref>(string_to_ref("{ null, 1234 }")), decode_exception);
  BOOST_CHECK_THROW(encoded_value<ref>(string_to_ref("[ null, 123 ] ")), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_not_validate_encode_context) {
  encoded_value<>(string_to_context("[ null, 1234 ]"), encoded_value<>::unsafe_unchecked());
  encoded_value<>(string_to_context("{ null, 1234 }"), encoded_value<>::unsafe_unchecked());
  encoded_value<>(string_to_context("[ null, 123 ] "), encoded_value<>::unsafe_unchecked());
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_implicitly_cast_to_string) {
  const auto json = std::string("[ null, 1234 ]");
  const encoded_value<> value(json);
  const std::string string = value;
  BOOST_CHECK_EQUAL(string, json);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_implicitly_cast_to_vector) {
  const auto json = string_to_vector("[ null, 1234 ]");
  const encoded_value<std::vector<char>> value(json);
  const std::vector<char> vector = value;
  BOOST_CHECK(vector == json);
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_implicitly_cast_to_ref) {
  const auto json = string_to_ref("[ null, 1234 ]");
  const encoded_value<ref> value(json);
  const ref data_ref = value;
  BOOST_CHECK(data_ref.data() == json.data());
  BOOST_CHECK(data_ref.size() == json.size());
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_move_to_string) {
  const auto json = std::string("[ null, 1234 ]");
  encoded_value<> value(json);
  take_rvalue_string(std::move(value));
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_forward_data_call) {
  const auto json = string_to_ref("[ null, 1234 ]");
  const encoded_value<ref> value(json);
  BOOST_CHECK(json.data() == value.data());
}

BOOST_AUTO_TEST_CASE(json_encoded_value_should_forward_size_call) {
  const auto json = string_to_ref("[ null, 1234 ]");
  const encoded_value<ref> value(json);
  BOOST_CHECK(json.size() == value.size());
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
