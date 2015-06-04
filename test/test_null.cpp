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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/null.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

BOOST_AUTO_TEST_CASE(json_codec_null_should_encode) {
  const auto codec = null_t();

  buffer buffer;
  detail::writer writer(buffer);
  codec.encode(null_type(), writer);
  const std::string result(buffer.data(), buffer.size());
  BOOST_CHECK_EQUAL(result, "null");
}

BOOST_AUTO_TEST_CASE(json_codec_null_should_decode_null) {
  const auto codec = null_t();
  const char *null = "null ";
  auto ctx = decoding_context(null, null + 5);
  const auto original_ctx = ctx;
  codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 4);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_codec_null_should_not_decode_otherwise) {
  const auto codec = null_t();
  const char *not_null = "nul";
  auto ctx = decoding_context(not_null, not_null + 3);
  BOOST_CHECK_THROW(codec.decode(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_codec_null_should_construct_with_helper) {
  null();
}

BOOST_AUTO_TEST_CASE(json_codec_null_should_construct_with_default_codec) {
  default_codec<null_type>();
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
