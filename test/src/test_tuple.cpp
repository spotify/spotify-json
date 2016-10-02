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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/omit.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/codec/tuple.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename Parsed = std::tuple<>>
Parsed tuple_parse(const char *not_array) {
  const auto codec = default_codec<Parsed>();
  auto ctx = decode_context(not_array, not_array + strlen(not_array));
  const auto result = codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  return result;
}

template <typename Parsed = std::tuple<>>
void tuple_parse_should_fail(const char *not_array) {
  const auto codec = default_codec<Parsed>();
  auto ctx = decode_context(not_array, not_array + strlen(not_array));
  BOOST_CHECK_THROW(codec.decode(ctx), decode_exception);
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_construct_with_tuple_helper) {
  tuple();
  tuple(boolean());
  tuple(boolean(), string());
}

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_construct_with_pair_helper) {
  pair(boolean(), string());
}

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_construct_tuple_with_default_codec) {
  default_codec<std::tuple<>>();
  default_codec<std::tuple<bool>>();
  default_codec<std::tuple<bool, int>>();
  default_codec<std::tuple<bool, int, std::string>>();
}

/*
 * Tuple Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_decode_tuple_success) {
  tuple_parse<std::tuple<>>("[]");
  tuple_parse<std::tuple<>>("[ ]");

  BOOST_CHECK((tuple_parse<std::tuple<int>>("[1]")) == std::make_tuple(1));
  BOOST_CHECK((tuple_parse<std::tuple<int>>("[ 1]")) == std::make_tuple(1));
  BOOST_CHECK((tuple_parse<std::tuple<int>>("[1 ]")) == std::make_tuple(1));
  BOOST_CHECK((tuple_parse<std::tuple<int>>("[ 1 ]")) == std::make_tuple(1));

  BOOST_CHECK((tuple_parse<std::tuple<int, bool>>("[1,false]")) == std::make_tuple(1, false));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool>>("[1 ,false]")) == std::make_tuple(1, false));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool>>("[1, false]")) == std::make_tuple(1, false));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool>>("[1 , false]")) == std::make_tuple(1, false));

  BOOST_CHECK((tuple_parse<std::tuple<int, bool, std::string>>("[1,false,\"a\"]")) == std::make_tuple(1, false, std::string("a")));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool, std::string>>("[1 ,false,\"a\"]")) == std::make_tuple(1, false, std::string("a")));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool, std::string>>("[1, false,\"a\"]")) == std::make_tuple(1, false, std::string("a")));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool, std::string>>("[1,false  , \"a\" ]")) == std::make_tuple(1, false, std::string("a")));
  BOOST_CHECK((tuple_parse<std::tuple<int, bool, std::string>>("[1 , false , \"a\" ]")) == std::make_tuple(1, false, std::string("a")));
}

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_not_skip_any_whitespace_at_end) {
  const auto codec = default_codec<std::tuple<>>();
  const auto json = "[] ";
  auto context = decode_context(json, json + strlen(json));
  codec.decode(context);
  BOOST_CHECK_EQUAL(context.position + 1, context.end);
}

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_not_decode_tuple_failure) {
  tuple_parse_should_fail<std::tuple<>>("");
  tuple_parse_should_fail<std::tuple<>>("[");
  tuple_parse_should_fail<std::tuple<>>("[[");
  tuple_parse_should_fail<std::tuple<>>("[false");
  tuple_parse_should_fail<std::tuple<>>("[false,true,]");
  tuple_parse_should_fail<std::tuple<>>("[false,true,");
  tuple_parse_should_fail<std::tuple<>>("[false]");
  tuple_parse_should_fail<std::tuple<>>("[false,1]");

  tuple_parse_should_fail<std::tuple<bool>>("");
  tuple_parse_should_fail<std::tuple<bool>>("[");
  tuple_parse_should_fail<std::tuple<bool>>("[[");
  tuple_parse_should_fail<std::tuple<bool>>("[false");
  tuple_parse_should_fail<std::tuple<bool>>("[false,true,]");
  tuple_parse_should_fail<std::tuple<bool>>("[false,true,");
  tuple_parse_should_fail<std::tuple<bool>>("[1]");
  tuple_parse_should_fail<std::tuple<bool>>("[false,1]");

  tuple_parse_should_fail<std::tuple<int, bool>>("");
  tuple_parse_should_fail<std::tuple<int, bool>>("[");
  tuple_parse_should_fail<std::tuple<int, bool>>("[[");
  tuple_parse_should_fail<std::tuple<int, bool>>("[false");
  tuple_parse_should_fail<std::tuple<int, bool>>("[false,true,]");
  tuple_parse_should_fail<std::tuple<int, bool>>("[false,true,");
  tuple_parse_should_fail<std::tuple<int, bool>>("[1]");
  tuple_parse_should_fail<std::tuple<int, bool>>("[false,1]");
  tuple_parse_should_fail<std::tuple<int, bool>>("[1,false,1]");
}

/*
 * Pair Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_decode_pair_success) {
  BOOST_CHECK((tuple_parse<std::pair<int, bool>>("[1,false]")) == std::make_pair(1, false));
  BOOST_CHECK((tuple_parse<std::pair<int, bool>>("[1 ,false]")) == std::make_pair(1, false));
  BOOST_CHECK((tuple_parse<std::pair<int, bool>>("[1, false]")) == std::make_pair(1, false));
  BOOST_CHECK((tuple_parse<std::pair<int, bool>>("[1 , false]")) == std::make_pair(1, false));
}


BOOST_AUTO_TEST_CASE(json_codec_tuple_should_not_decode_pair_failure) {
  tuple_parse_should_fail<std::pair<int, bool>>("");
  tuple_parse_should_fail<std::pair<int, bool>>("[");
  tuple_parse_should_fail<std::pair<int, bool>>("[[");
  tuple_parse_should_fail<std::pair<int, bool>>("[false");
  tuple_parse_should_fail<std::pair<int, bool>>("[false,true,]");
  tuple_parse_should_fail<std::pair<int, bool>>("[false,true,");
  tuple_parse_should_fail<std::pair<int, bool>>("[1]");
  tuple_parse_should_fail<std::pair<int, bool>>("[false,1]");
  tuple_parse_should_fail<std::pair<int, bool>>("[1,false,1]");
}

/*
 * Tuple Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_encode_tuple) {
  BOOST_CHECK_EQUAL(encode(std::make_tuple()), "[]");
  BOOST_CHECK_EQUAL(encode(std::make_tuple(1)), "[1]");
  BOOST_CHECK_EQUAL(encode(std::make_tuple(1, true)), "[1,true]");
  BOOST_CHECK_EQUAL(encode(std::make_tuple(1, true, std::string("a"))), "[1,true,\"a\"]");
}

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_not_encode_omitted_elements_tuple) {
  const auto codec = tuple(omit<bool>(), omit<bool>());
  BOOST_CHECK_EQUAL(encode(codec, std::make_tuple(false, true)), "[]");
}

/*
 * Pair Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_encode_pair) {
  BOOST_CHECK_EQUAL(encode(std::make_pair(1, true)), "[1,true]");
}

BOOST_AUTO_TEST_CASE(json_codec_tuple_should_not_encode_omitted_elements_pair) {
  const auto codec = tuple(omit<bool>(), omit<bool>());
  BOOST_CHECK_EQUAL(encode(codec, std::make_pair(false, true)), "[]");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
