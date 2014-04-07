// Copyright (c) 2014 Felix Bruns and Jamie Kirkpatrick.

#pragma once

#include <boost/optional.hpp>

#include "json_pair.hpp"

namespace json {

template<typename StreamType, typename T>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const boost::optional<T> &optional) {
  if (optional) {
    writer << optional.get();
  }
  return writer;
}

template<typename StreamType, typename K, typename V>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const pair<K, boost::optional<V> > &pair) {
  if (pair.value) {
    writer.add_pair(pair.key, pair.value.get());
  }
  return writer;
}

template<typename StreamType, typename K, typename V>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::pair<K, boost::optional<V> > &pair) {
  if (pair.second) {
    writer.add_pair(pair.first, pair.second.get());
  }
  return writer;
}

}  // namespace json
