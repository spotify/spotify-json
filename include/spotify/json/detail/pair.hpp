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

namespace spotify {
namespace json {
namespace detail {

template<typename stream_type, typename options_type>
class basic_writer;

/**
 * \brief Represents a JSON key-value pair.
 *
 * ATTENTION! Key and value are only referenced, not copied!
 */
template<typename K, typename V>
class pair {
 public:
  /**
   * \brief Construct a JSON key-value pair.
   */
  pair(const K &k, const V &v)
    : key(k), value(v) {}

  const K &key;
  const V &value;
};

/**
 * \brief Create a JSON key-value pair.
 */
template<typename K, typename V>
inline pair<K, V> make_pair(const K &key, const V &value) {
  return pair<K, V>(key, value);
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const pair<K, V> &pair) {
  return writer.add_pair(pair.key, pair.value);
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
