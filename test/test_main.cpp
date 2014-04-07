/*
 * Copyright (c) 2014 Spotify AB
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

#define BOOST_TEST_MODULE json_test

#include <string>
#include <map>
#include <utility>

#include <boost/test/unit_test.hpp>

#include <boost/optional.hpp>

#include "json_pair.hpp"
#include "json_writer.hpp"

#include "ext/json_boost.hpp"
#include "ext/json_std.hpp"

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_vector_with_boost_optional) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::vector<boost::optional<int> > vector;
  vector.push_back(boost::optional<int>(1));
  vector.push_back(boost::optional<int>(2));
  vector.push_back(boost::optional<int>());  // empty
  writer << vector;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[1,2]", json);
}

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_map_with_boost_optional) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::map<std::string, boost::optional<int> > map;
  map["a"] = boost::optional<int>(1);
  map["b"] = boost::optional<int>(2);
  map["c"] = boost::optional<int>();  // empty
  writer << map;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("{\"a\":1,\"b\":2}", json);
}

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_vector) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::vector<std::string> vector;
  vector.push_back("a");
  vector.push_back("b");
  vector.push_back("c");
  writer << vector;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[\"a\",\"b\",\"c\"]", json);
}

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_deque) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::deque<std::string> deque;
  deque.push_back("a");
  deque.push_back("b");
  deque.push_back("c");
  writer << deque;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[\"a\",\"b\",\"c\"]", json);
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
