// Copyright (c) 2014 Felix Bruns.

#define BOOST_TEST_MODULE json_test

#include <string>
#include <map>
#include <utility>

#include <boost/test/unit_test.hpp>

#include <boost/optional.hpp>

#include "json_writer.hpp"
#include "ext/json_std.hpp"

namespace json {

template<typename StreamType, typename K, typename V>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::pair<K, boost::optional<V> > &pair) {
  if (pair.second) {
    writer.add_pair(pair.first, pair.second.get());
  }
  return writer;
}

}  // namespace json

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_pair_with_optional) {
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
