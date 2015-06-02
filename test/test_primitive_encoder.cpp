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

#include <type_traits>

#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/primitive_encoder.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

struct new_type {
};

}  // namespace

BOOST_AUTO_TEST_CASE(json_primitive_encoder_should_have_object_type) {
  static_assert(std::is_same<
      primitive_encoder<new_type>::object_type,
      new_type>::value, "primitive_encoder should have object_type");
}

BOOST_AUTO_TEST_CASE(json_primitive_encoder_should_encode) {
  const primitive_encoder<bool> encoder{};
  buffer buffer;
  writer writer(buffer);
  encoder.encode(true, writer);
  const std::string result(buffer.data(), buffer.size());
  BOOST_CHECK_EQUAL(result, "true");
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
