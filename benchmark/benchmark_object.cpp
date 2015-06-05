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

#include <string>

#include <sstream>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/object.hpp>
#include <spotify/json/decode_exception.hpp>
#include <spotify/json/encode_decode.hpp>

#include "benchmark.hpp"

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

struct struct_t {
  int integer;
};

BOOST_AUTO_TEST_CASE(benchmark_json_codec_string_parse_object_with_required_fields) {
  codec::object<struct_t> codec;
  std::stringstream json_ss;
  json_ss << "{";

  for (char c = 'a'; c <= 'z'; c++) {
    const auto key = std::string(&c, 1);
    codec.required(key, &struct_t::integer);
    json_ss << '"' << c << '"' << ":0,";
  }

  json_ss << '"' << '.' << '"' << ":0}";
  const auto json = json_ss.str();

  JSON_BENCHMARK(1e5, [=]{
    auto context = decoding_context(json.data(), json.data() + json.size());
    codec.decode(context);
  });
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
