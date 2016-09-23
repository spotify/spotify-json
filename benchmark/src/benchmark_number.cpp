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

#include <spotify/json/codec/number.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>
#include <spotify/json/encode_context.hpp>

#include <spotify/json/benchmark/benchmark.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

BOOST_AUTO_TEST_CASE(benchmark_json_codec_number_encode_positive_int32_t) {
  const auto codec = number<int32_t>();
  JSON_BENCHMARK(1e5, [=]{
    auto context = encode_context();
    for (int32_t i = 0; i < 10000000; i += 48071) {
      codec.encode(context, i);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_number_encode_positive_uint32_t) {
  const auto codec = number<uint32_t>();
  JSON_BENCHMARK(1e5, [=]{
    auto context = encode_context();
    for (uint32_t i = 0; i < 10000000; i += 48071) {
      codec.encode(context, i);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_number_encode_positive_int64_t) {
  const auto codec = number<int64_t>();
  JSON_BENCHMARK(1e5, [=]{
    auto context = encode_context();
    for (int64_t i = 0; i < 10000000; i += 48071) {
      codec.encode(context, i);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_number_encode_positive_uint64_t) {
  const auto codec = number<uint64_t>();
  JSON_BENCHMARK(1e5, [=]{
    auto context = encode_context();
    for (uint64_t i = 0; i < 10000000; i += 48071) {
      codec.encode(context, i);
      context.clear();
    }
  });
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
