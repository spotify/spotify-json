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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

void boolean_parse_should_fail(const char *not_boolean) {
  const auto codec = boolean_t();
  auto ctx = decoding_context(not_boolean, not_boolean + strlen(not_boolean));
  BOOST_CHECK_THROW(codec.decode(ctx), decode_exception);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_encode_true) {
  BOOST_CHECK_EQUAL(encode(true), "true");
}

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_encode_false) {
  BOOST_CHECK_EQUAL(encode(false), "false");
}

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_decode_true) {
  const auto codec = boolean_t();
  const char *boolean = "true ";
  auto ctx = decoding_context(boolean, boolean + 5);
  const auto original_ctx = ctx;
  BOOST_CHECK_EQUAL(codec.decode(ctx), true);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 4);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_decode_false) {
  const auto codec = boolean_t();
  const char *boolean = "false ";
  auto ctx = decoding_context(boolean, boolean + 6);
  const auto original_ctx = ctx;
  BOOST_CHECK_EQUAL(codec.decode(ctx), false);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 5);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_not_decode_otherwise) {
  boolean_parse_should_fail("");
  boolean_parse_should_fail("TRUE");
  boolean_parse_should_fail("tru");
  boolean_parse_should_fail("fals");
}

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_construct_with_helper) {
  boolean();
}

BOOST_AUTO_TEST_CASE(json_codec_boolean_should_construct_with_default_codec) {
  default_codec<bool>();
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
