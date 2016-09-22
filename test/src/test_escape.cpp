/*
 * Copyright (c) 2014-2016 Spotify AB
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

#include <boost/range/irange.hpp>
#include <boost/range/join.hpp>
#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/escape.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

using namespace std;
using namespace boost;

void check_escaped(const std::string &expected, const std::string &input) {
  encode_context context;
  const auto begin = reinterpret_cast<const uint8_t *>(input.data());
  write_escaped(context, begin, begin + input.size());
  BOOST_CHECK_EQUAL(expected, std::string(reinterpret_cast<const char *>(context.data()), context.size()));
}

BOOST_AUTO_TEST_CASE(json_write_escaped_should_escape_special_characters) {
  check_escaped("\\\\", "\\");  // quotation mark
  check_escaped("\\\"", "\"");  // reverse solidus
}

BOOST_AUTO_TEST_CASE(json_write_escaped_should_not_escape_solidus) {
  check_escaped("/", "/");  // solidus
}

BOOST_AUTO_TEST_CASE(json_write_escaped_should_escape_special_control_characters) {
  check_escaped("\\b", "\b");  // backspace
  check_escaped("\\t", "\t");  // tab
  check_escaped("\\n", "\n");  // line feed
  check_escaped("\\f", "\f");  // form feed
  check_escaped("\\r", "\r");  // carriage return
}

BOOST_AUTO_TEST_CASE(json_write_escaped_should_escape_other_control_characters) {
  const char *hex = "0123456789ABCDEF";
  const auto range(join(join(irange(0x00, 0x07), irange(0x0B, 0x0B)), irange(0x0E, 0x1F)));

  for (char ch : range) {
    const std::string hi(1, hex[ch >> 4]);
    const std::string lo(1, hex[ch & 0x0F]);
    check_escaped("\\u00" + hi + lo, std::string(1, ch));
  }
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
