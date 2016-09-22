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

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/any.hpp>
#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/encode.hpp>

#include <spotify/json/test/only_true.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

bool any_parse(const char *str) {
  any_t<bool> codec = any_t<bool>(boolean());
  auto ctx = decode_context(str, str + strlen(str));
  const auto result = codec.decode(ctx);

  BOOST_CHECK_EQUAL(ctx.position, ctx.end);
  return result;
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_any_should_encode) {
  any_t<bool> codec = any_t<bool>(boolean());
  BOOST_CHECK_EQUAL(encode(codec, true), "true");
  BOOST_CHECK_EQUAL(encode(codec, false), "false");
}

BOOST_AUTO_TEST_CASE(json_any_should_decode) {
  BOOST_CHECK_EQUAL(any_parse("true"), true);
  BOOST_CHECK_EQUAL(any_parse("false"), false);
}

BOOST_AUTO_TEST_CASE(json_any_should_construct_with_helper) {
  any(boolean());
}

BOOST_AUTO_TEST_CASE(json_any_should_respect_should_encode) {
  auto codec = any(only_true_t());
  BOOST_CHECK(codec.should_encode(true));
  BOOST_CHECK(!codec.should_encode(false));
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
