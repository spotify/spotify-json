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

#include <boost/test/unit_test.hpp>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/detail/skip_chars.hpp>

#include <spotify/json/benchmark/benchmark.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

std::string generate_simple_string(size_t size) {
  std::string string;
  for (size_t i = 0; i < size; i++) {
    char c;
    switch (i % 3) {
      case 0: c = '0' + (i % 10); break;
      case 1: c = 'a' + (i % ('z' - 'a')); break;
      case 2: c = 'A' + (i % ('Z' - 'A')); break;
    }
    string.append(&c, 1);
  }
  return string;
}

BOOST_AUTO_TEST_CASE(benchmark_json_detail_skip_any_simple_characters) {
  const auto json = generate_simple_string(8192);
  volatile size_t n = 0;
  JSON_BENCHMARK(1e6, [&]{
    auto context = decode_context(json.data(), json.data() + json.size());
    *const_cast<bool *>(&context.has_sse42) = false;
    detail::skip_any_simple_characters(context);
    n += context.offset();
  });
}

#if defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_CASE(benchmark_json_detail_skip_any_simple_characters_sse42) {
  const auto json = generate_simple_string(8192);
  volatile size_t n = 0;
  JSON_BENCHMARK(1e6, [&]{
    auto context = decode_context(json.data(), json.data() + json.size());
    detail::skip_any_simple_characters(context);
    n += context.offset();
  });
}

#endif  // defined(json_arch_x86_sse42)

std::string generate_whitespace_string(size_t size) {
  std::string string;
  for (size_t i = 0; i < size; i++) {
    char c;
    switch (i % 4) {
      case 0: c = ' '; break;
      case 1: c = '\t'; break;
      case 2: c = '\n'; break;
      case 3: c = '\r'; break;
    }
    string.append(&c, 1);
  }
  return string;
}

BOOST_AUTO_TEST_CASE(benchmark_json_detail_skip_any_whitespace) {
  const auto json = generate_whitespace_string(8192);
  volatile size_t n = 0;
  JSON_BENCHMARK(1e6, [&]{
    auto context = decode_context(json.data(), json.data() + json.size());
    *const_cast<bool *>(&context.has_sse42) = false;
    detail::skip_any_whitespace(context);
    n += context.offset();
  });
}

#if defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_CASE(benchmark_json_detail_skip_any_whitespace_sse42) {
  const auto json = generate_whitespace_string(8192);
  volatile size_t n = 0;
  JSON_BENCHMARK(1e6, [&]{
    auto context = decode_context(json.data(), json.data() + json.size());
    detail::skip_any_whitespace(context);
    n += context.offset();
  });
}

#endif  // defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
