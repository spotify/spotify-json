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

#include <spotify/json/codec/string.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/decode_exception.hpp>
#include <spotify/json/encode.hpp>

#include <spotify/json/benchmark/benchmark.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

std::string generate_simple_string(size_t size) {
  std::string string;
  string.reserve(size);
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

std::string generate_simple_json_string(size_t size) {
  return "\"" + generate_simple_string(size) + "\"";
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(benchmark_json_codec_string_decode_simple_long_string) {
  const auto codec = default_codec<std::string>();
  const auto json = generate_simple_json_string(10000);
  const auto json_begin = json.data();
  const auto json_end = json.data() + json.size();
  JSON_BENCHMARK(1e5, [=]{
    auto context = decode_context(json_begin, json_end);
    const auto decoded_string = codec.decode(context);
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_string_decode_simple_tiny_string) {
  const auto codec = default_codec<std::string>();
  const auto json = std::string("\"spotify:track:05341EWu6uHUg2BojF3Cyw\"");
  const auto json_begin = json.data();
  const auto json_end = json.data() + json.size();
  JSON_BENCHMARK(1e5, [=]{
    for (int i = 0; i < 100; i++) {
      auto context = decode_context(json_begin, json_end);
      const auto decoded_string = codec.decode(context);
    }
  });
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(benchmark_json_codec_string_encode_simple_long_string) {
  const auto codec = default_codec<std::string>();
  const auto string = generate_simple_string(10000);
  auto context = encode_context(string.size() + 2);
  JSON_BENCHMARK(1e5, [&]{
    codec.encode(context, string);
    context.clear();
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_string_encode_simple_tiny_string) {
  const auto codec = default_codec<std::string>();
  const auto string = std::string("spotify:track:05341EWu6uHUg2BojF3Cyw");
  auto context = encode_context(string.size() + 2);
  JSON_BENCHMARK(1e5, [&]{
    for (int i = 0; i < 100; i++) {
      codec.encode(context, string);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
