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

#include <sstream>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/object.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/decode_exception.hpp>
#include <spotify/json/encode.hpp>

#include <spotify/json/benchmark/benchmark.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

struct struct_t {
  int integer;
};

codec::object_t<struct_t> required_codec(size_t n) {
  auto codec = codec::object<struct_t>();
  const size_t num_letters = 'z' - 'a';
  for (size_t i = 0; i < n; i++) {
    const auto c = static_cast<char>('a' + (i % num_letters));
    const auto m = (i / num_letters);
    std::stringstream key_ss;
    key_ss << c << m;
    codec.required(key_ss.str(), &struct_t::integer);
  }
  return codec;
}

std::string make_json(size_t n) {
  std::stringstream json_ss;
  json_ss << "{";

  const size_t num_letters = 'z' - 'a';
  for (size_t i = 0; i < n; i++) {
    const auto c = static_cast<char>('a' + (i % num_letters));
    const auto m = (i / num_letters);
    std::stringstream key_ss;
    json_ss << '"' << c << m << '"' << ":0,";
  }

  json_ss << '"' << '.' << '"' << ":0}";
  return json_ss.str();
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_object_decode_with_few_required_fields) {
  const auto codec = required_codec(50);
  const auto json = make_json(50);

  JSON_BENCHMARK(1e5, [=]{
    auto context = decoding_context(json.data(), json.data() + json.size());
    codec.decode(context);
  });
}

BOOST_AUTO_TEST_CASE(benchmark_json_codec_object_decode_with_many_required_fields) {
  const auto codec = required_codec(1000);
  const auto json = make_json(1000);

  JSON_BENCHMARK(1e4, [=]{
    auto context = decoding_context(json.data(), json.data() + json.size());
    codec.decode(context);
  });
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
