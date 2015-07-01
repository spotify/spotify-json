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

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/encode_decode.hpp>
#include <spotify/json/extension/boost.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

class base_class {
 public:
  virtual ~base_class() = default;
};

class sub_class : public base_class {
};

codec::object<sub_class> sub_codec() {
  codec::object<sub_class> codec;
  return codec;
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_boost_shared_ptr_should_decode) {
  const auto obj = decode<boost::shared_ptr<std::string>>("\"hello\"");
  BOOST_REQUIRE(obj);
  BOOST_CHECK_EQUAL(*obj, "hello");
}

BOOST_AUTO_TEST_CASE(json_codec_boost_cast_pointer_should_construct_with_helper) {
  const boost::shared_ptr<base_class> ptr = boost::make_shared<sub_class>();
  const auto codec = codec::cast<boost::shared_ptr<base_class>>(boost_shared_ptr(sub_codec()));
  BOOST_CHECK_EQUAL(encode(codec, ptr), "{}");
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
