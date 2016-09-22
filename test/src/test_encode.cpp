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

#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

struct custom_obj {
  std::string val;
};

codec::object_t<custom_obj> custom_codec() {
  auto codec = codec::object<custom_obj>();
  codec.required("a", &custom_obj::val);
  return codec;
}

}

template <>
struct default_codec_t<custom_obj> {
  static codec::object_t<custom_obj> codec() {
    auto codec = codec::object<custom_obj>();
    codec.required("x", &custom_obj::val);
    return codec;
  }
};

BOOST_AUTO_TEST_CASE(json_encode_should_encode_into_string_with_custom_codec) {
  custom_obj obj;
  obj.val = "c";
  BOOST_CHECK_EQUAL(encode(custom_codec(), obj), R"({"a":"c"})");
}

BOOST_AUTO_TEST_CASE(json_encode_should_encode_into_string) {
  custom_obj obj;
  obj.val = "d";
  BOOST_CHECK_EQUAL(encode(obj), R"({"x":"d"})");
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
