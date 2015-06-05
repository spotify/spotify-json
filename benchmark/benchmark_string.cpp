/*
 * Copyright (c) 2015 Spotify AB
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

#include <chrono>
#include <string>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/string.hpp>
#include <spotify/json/decode_exception.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

template <typename test_fn>
void benchmark(const char *name, const test_fn &test) {
  using namespace std::chrono;
  unsigned count = 0;

  const auto before = high_resolution_clock::now();
  for (unsigned i = 0; i < 100000; i++) {
    count++;
    test();
  }
  const auto after = high_resolution_clock::now();

  const auto duration = (after - before);
  const auto duration_ms = duration_cast<milliseconds>(duration).count();
  const auto duration_ms_avg = (duration_ms / static_cast<double>(count));
  std::cerr << name << ": " << duration_ms_avg << " ms avg (" << count << " runs)" << std::endl;
}

#define JSON_BENCHMARK(test) benchmark(typeid(*this).name(), (test))

std::string string_parse(const char *string) {
  const auto codec = default_codec<std::string>();
  auto ctx = decoding_context(string, string + strlen(string));
  const auto result = codec.decode(ctx);
  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  return result;
}

std::string generate_simple_string(size_t size) {
  std::string string("\"");
  for (size_t i = 0; i < size; i++) {
    char c;
    switch (i % 3) {
      case 0: c = '0' + (i % 10); break;
      case 1: c = 'a' + (i % ('z' - 'a')); break;
      case 2: c = 'A' + (i % ('Z' - 'A')); break;
    }
    string.append(&c, 1);
  }
  string.append("\"");
  return string;
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_string_parse_nonescaped) {
  const auto codec = default_codec<std::string>();
  const auto json = generate_simple_string(10000);
  JSON_BENCHMARK([=]{
    auto context = decoding_context(json.data(), json.data() + json.size());
    codec.decode(context);
  });
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
