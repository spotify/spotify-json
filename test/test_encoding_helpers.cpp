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

#include <type_traits>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/detail/encoding_helpers.hpp>

#include "only_true.hpp"

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

BOOST_AUTO_TEST_CASE(json_encoding_helpers_should_encode_by_default) {
  BOOST_CHECK(should_encode(codec::boolean(), true));
}

BOOST_AUTO_TEST_CASE(json_encoding_helpers_should_encode_should_respect_should_encode) {
  BOOST_CHECK(should_encode(codec::only_true_t(), true));
  BOOST_CHECK(!should_encode(codec::only_true_t(), false));
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
