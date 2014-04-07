// Copyright (c) 2014 Felix Bruns and Jamie Kirkpatrick.

#pragma once

#include <deque>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace json {

namespace detail {

template<typename WriterType, typename Iterable>
inline WriterType &write_array(WriterType &writer, const Iterable &iterable) {
  const WriterType::scoped_array array(writer);
  for (Iterable::const_iterator it = iterable.begin(); it != iterable.end(); ++it) {
    writer << *it;
  }
  return writer;
}

template<typename WriterType, typename Iterable>
inline WriterType &write_object(WriterType &writer, const Iterable &iterable) {
  const WriterType::scoped_object object(writer);
  for (Iterable::const_iterator it = iterable.begin(); it != iterable.end(); ++it) {
    writer << *it;
  }
  return writer;
}

}  // namespace detail

template<typename StreamType, typename T>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::vector<T> &vector) {
  return detail::write_array(writer, vector);
}

template<typename StreamType, typename T>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::deque<T> &deque) {
  return detail::write_array(writer, deque);
}

template<typename StreamType, typename T>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::set<T> &set) {
  return detail::write_array(writer, set);
}

template<typename StreamType, typename K, typename V>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::pair<K, V> &pair) {
  return writer.add_pair(pair.first, pair.second);
}

template<typename StreamType, typename K, typename V>
basic_writer<StreamType> &operator <<(basic_writer<StreamType> &writer, const std::map<K, V> &map) {
  return detail::write_object(writer, map);
}

}  // namespace json
