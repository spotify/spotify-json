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

#include <spotify/json/detail/pair.hpp>
#include <spotify/json/detail/writer.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

BOOST_AUTO_TEST_CASE(json_writer_add_pair) {
  json::buffer buffer;
  writer w(buffer);

  w.add_object([](writer &w) {
    w.add_pair("a", 0);
  });

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("{\"a\":0}", json);
}

BOOST_AUTO_TEST_CASE(json_writer_add_key) {
  json::buffer buffer;
  writer w(buffer);

  w.add_object([](writer &w) {
    w.add_key("a");
    w << 0;
  });

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("{\"a\":0}", json);
}

BOOST_AUTO_TEST_CASE(json_writer_write_buffer) {
  json::buffer buffer;
  writer w(buffer);

  json::buffer data;
  data.write("foobar", 6);
  w << data;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("foobar", json);
}

BOOST_AUTO_TEST_CASE(json_writer_write_raw_value_should_add_separator) {
  json::buffer buffer;
  writer w(buffer);

  w.add_array([](writer &w) {
    w.write_raw_value("{}", 2);
    w.write_raw_value("{}", 2);
    w.write_raw_value("{}", 2);
  });

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[{},{},{}]", json);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
