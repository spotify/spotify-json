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

#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/encoding_context.hpp>

#include <spotify/json/benchmark/benchmark.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(benchmark_json_codec_boolean_decode_false) {
  const auto codec = default_codec<bool>();
  const auto json = std::string("false");
  const auto json_begin = json.data();
  const auto json_end = json.data() + json.size();
  JSON_BENCHMARK(1e5, [=]{
    for (int i = 0; i < 100; i++) {
      auto context = decoding_context(json_begin, json_end);
      codec.decode(context);
    }
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_boolean_decode_true) {
  const auto codec = default_codec<bool>();
  const auto json = std::string("true");
  const auto json_begin = json.data();
  const auto json_end = json.data() + json.size();
  JSON_BENCHMARK(1e5, [=]{
    for (int i = 0; i < 100; i++) {
      auto context = decoding_context(json_begin, json_end);
      codec.decode(context);
    }
  });
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(benchmark_json_codec_boolean_encode_false) {
  const auto codec = boolean();
  JSON_BENCHMARK(1e5, [=]{
    auto context = encoding_context();
    for (auto i = 0; i < 1000; i++) {
      codec.encode(context, false);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_boolean_encode_true) {
  const auto codec = boolean();
  JSON_BENCHMARK(1e5, [=]{
    auto context = encoding_context();
    for (auto i = 0; i < 1000; i++) {
      codec.encode(context, true);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
