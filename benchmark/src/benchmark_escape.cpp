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

#include <spotify/json/detail/escape.hpp>

#include <spotify/json/benchmark/benchmark.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

std::string generate_string(size_t size, bool add_special_characters) {
  std::string string;
  for (size_t i = 0; i < size; i++) {
    char c;
    if (add_special_characters && (i % 0x20) == 0) {
      c = 0x01 + i % (0x1f - 0x01);
    } else {
      switch (i % 3) {
        case 0: c = '0' + (i % 10); break;
        case 1: c = 'a' + (i % ('z' - 'a')); break;
        case 2: c = 'A' + (i % ('Z' - 'A')); break;
      }
    }
    string.append(&c, 1);
  }
  return string;
}

void check_escaped(const std::string &expected, const std::string &input) {
  encode_context context;
  write_escaped(context, input.data(), input.data() + input.size());
  const auto x = context.size();
  BOOST_CHECK_EQUAL(expected, std::string(context.data(), x));
}

BOOST_AUTO_TEST_CASE(benchmark_json_detail_write_escaped_simple_string) {
  const auto input = generate_string(8192, false);
  const auto begin = input.data();

  volatile size_t n = 0;
  JSON_BENCHMARK(1e5, [&] {
    encode_context context;
    *const_cast<bool *>(&context.has_sse42) = false;
    write_escaped(context, begin, begin + input.size());
    n += context.size();
  });
}

#if defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_CASE(benchmark_json_detail_write_escaped_simple_string_sse42) {
  const auto input = generate_string(8192, false);
  const auto begin = input.data();

  volatile size_t n = 0;
  JSON_BENCHMARK(1e5, [&] {
    encode_context context;
    write_escaped(context, begin, begin + input.size());
    n += context.size();
  });
}

#endif  // defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_CASE(benchmark_json_detail_write_escaped_complex_string) {
  const auto input = generate_string(8192, true);
  const auto begin = input.data();

  volatile size_t n = 0;
  JSON_BENCHMARK(1e5, [&] {
    encode_context context;
    *const_cast<bool *>(&context.has_sse42) = false;
    write_escaped(context, begin, begin + input.size());
    n += context.size();
  });
}

#if defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_CASE(benchmark_json_detail_write_escaped_complex_string_sse42) {
  const auto input = generate_string(8192, true);
  const auto begin = input.data();

  volatile size_t n = 0;
  JSON_BENCHMARK(1e5, [&] {
    encode_context context;
    write_escaped(context, begin, begin + input.size());
    n += context.size();
  });
}

#endif  // defined(json_arch_x86_sse42)

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
