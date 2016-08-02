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

#pragma once

#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace detail {

template<typename WriterType, typename Iterable>
inline WriterType &write_array(WriterType &writer, const Iterable &iterable) {
  const typename WriterType::scoped_array array(writer);
  for (typename Iterable::const_iterator it = iterable.begin(); it != iterable.end(); ++it) {
    writer << *it;
  }
  return writer;
}

template<typename WriterType, typename Iterable>
inline WriterType &write_object(WriterType &writer, const Iterable &iterable) {
  const typename WriterType::scoped_object object(writer);
  for (typename Iterable::const_iterator it = iterable.begin(); it != iterable.end(); ++it) {
    writer << *it;
  }
  return writer;
}

template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::vector<T> &vector) {
  return write_array(writer, vector);
}

template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::deque<T> &deque) {
  return write_array(writer, deque);
}

template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::set<T> &set) {
  return write_array(writer, set);
}

template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::unordered_set<T> &set) {
  return write_array(writer, set);
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::pair<K, V> &pair) {
  return writer.add_pair(pair.first, pair.second);
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::map<K, V> &map) {
  return write_object(writer, map);
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::unordered_map<K, V> &map) {
  return write_object(writer, map);
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
