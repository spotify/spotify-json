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

#include <spotify/json/codec/cast.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template<typename T>
class dummy_t final {
 public:
  using object_type = T;

  void encode(object_type value, writer &w) const {
    w << false;
  }

  object_type decode(decoding_context &context) const {
    return new T();
  }
};

class base_class {
 public:
  virtual ~base_class() = default;
};

class sub_class : public base_class {
};

object_t<sub_class> sub_codec() {
  auto codec = object<sub_class>();
  return codec;
}

template<typename Codec>
typename Codec::object_type test_decode(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK_EQUAL(c.position, c.end);
  return obj;
}

}  // namespace

/*
 * Raw pointer
 */

BOOST_AUTO_TEST_CASE(json_codec_cast_pointer_should_construct) {
  cast_t<base_class *, dummy_t<sub_class *>>{dummy_t<sub_class *>()};
}

BOOST_AUTO_TEST_CASE(json_codec_cast_pointer_should_construct_with_helper) {
  cast<base_class *>(dummy_t<sub_class *>());
}

BOOST_AUTO_TEST_CASE(json_codec_cast_pointer_should_encode) {
  const std::shared_ptr<base_class> ptr = std::make_shared<sub_class>();
  const auto codec = cast<std::shared_ptr<base_class>>(shared_ptr(sub_codec()));
  BOOST_CHECK_EQUAL(encode(codec, ptr), "{}");
}

BOOST_AUTO_TEST_CASE(json_codec_cast_pointer_should_decode) {
  const std::shared_ptr<base_class> ptr = std::make_shared<sub_class>();
  const auto codec = cast<std::shared_ptr<base_class>>(shared_ptr(sub_codec()));
  test_decode(codec, "{}");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
