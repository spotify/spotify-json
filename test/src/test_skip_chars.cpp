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

#include <cstdlib>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/skip_chars.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

std::string generate(const std::string &tpl, const std::size_t count) {
  std::string ws;
  ws.reserve(count);
  for (std::size_t i = 0; i < count; i++) {
    ws += tpl[i % tpl.size()];
  }
  return ws;
}

template <void (*function)(decode_context &)>
void verify_skip_any(
    const bool use_sse,
    const std::string &json,
    const std::size_t prefix = 0,
    const std::size_t suffix = 0) {
  auto context = decode_context(json.data() + prefix, json.data() + json.size());
  *const_cast<bool *>(&context.has_sse42) &= use_sse;
  const auto original_context = context;
  function(context);
  BOOST_CHECK_EQUAL(
      reinterpret_cast<intptr_t>(context.position),
      reinterpret_cast<intptr_t>(original_context.end - suffix));
  BOOST_CHECK_EQUAL(
      reinterpret_cast<intptr_t>(context.end),
      reinterpret_cast<intptr_t>(original_context.end));
}

template <void (*function)(decode_context &)>
void verify_skip_empty_nullptr(const bool use_sse) {
  auto context = decode_context(nullptr, nullptr);
  *const_cast<bool *>(&context.has_sse42) &= use_sse;
  function(context);
  BOOST_CHECK(context.position == nullptr);
  BOOST_CHECK(context.end == nullptr);
}

using true_false = boost::mpl::list<boost::true_type, boost::false_type>;

}  // namespace

/*
 * skip_any_simple_characters
 */

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_simple_characters, use_sse, true_false) {
  for (auto n = 0; n < 1024; n++) {
    const auto ws = generate("abcdefghIJKLMNOP:-,;'^¨´`xyz", n);
    const auto with_prefix = "\\" + ws;
    const auto with_suffix = ws + "\"abcde";
    verify_skip_any<skip_any_simple_characters>(use_sse::value, ws);
    verify_skip_any<skip_any_simple_characters>(use_sse::value, with_prefix, 1);
    verify_skip_any<skip_any_simple_characters>(use_sse::value, with_suffix, 0, 6);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_simple_characters_null_byte_in_string,
                              use_sse,
                              true_false) {
  alignas(16) char input_data[17] = "a\0\"\"\"\"\"\"\"\"\"\"\"\"\"\"";
  auto context = decode_context(input_data, input_data + 16);
  *const_cast<bool *>(&context.has_sse42) &= use_sse::value;
  skip_any_simple_characters(context);
  BOOST_CHECK_EQUAL(context.position - input_data, 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_simple_characters_with_empty_string,
                              use_sse,
                              true_false) {
  verify_skip_empty_nullptr<skip_any_simple_characters>(use_sse::value);
}

/*
 * skip_any_whitespace
 */

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_space, use_sse, true_false) {
  for (auto n = 0; n < 1024; n++) {
    const auto ws = generate(" ", n);
    const auto with_prefix = "}" + ws;
    const auto with_suffix = ws + "{ ";
    verify_skip_any<skip_any_whitespace>(use_sse::value, ws);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_prefix, 1);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_suffix, 0, 2);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_tabs, use_sse, true_false) {
  for (auto n = 0; n < 1024; n++) {
    const auto ws = generate("\t", n);
    const auto with_prefix = "}" + ws;
    const auto with_suffix = ws + "{ ";
    verify_skip_any<skip_any_whitespace>(use_sse::value, ws);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_prefix, 1);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_suffix, 0, 2);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_carriage_return, use_sse, true_false) {
  for (auto n = 0; n < 1024; n++) {
    const auto ws = generate("\r", n);
    const auto with_prefix = "}" + ws;
    const auto with_suffix = ws + "{ ";
    verify_skip_any<skip_any_whitespace>(use_sse::value, ws);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_prefix, 1);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_suffix, 0, 2);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_line_feed, use_sse, true_false) {
  for (auto n = 0; n < 1024; n++) {
    const auto ws = generate("\n", n);
    const auto with_prefix = "}" + ws;
    const auto with_suffix = ws + "{ ";
    verify_skip_any<skip_any_whitespace>(use_sse::value, ws);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_prefix, 1);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_suffix, 0, 2);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_whitespace, use_sse, true_false) {
  for (auto n = 0; n < 1024; n++) {
    const auto ws = generate("\n\t\r\n", n);
    const auto with_prefix = "}" + ws;
    const auto with_suffix = ws + "{ ";
    verify_skip_any<skip_any_whitespace>(use_sse::value, ws);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_prefix, 1);
    verify_skip_any<skip_any_whitespace>(use_sse::value, with_suffix, 0, 2);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(json_skip_any_whitespace_with_empty_string, use_sse, true_false) {
  verify_skip_empty_nullptr<skip_any_whitespace>(use_sse::value);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
