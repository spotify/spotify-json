// Copyright (c) 2014 Felix Bruns.

#pragma once
#ifndef JSON_PAIR_HPP_
#define JSON_PAIR_HPP_

#include <string>

namespace json {

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

}  // namespace json

#endif  // JSON_PAIR_HPP_
