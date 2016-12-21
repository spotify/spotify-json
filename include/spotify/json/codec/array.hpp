/*
 * Copyright (c) 2015-2016 Spotify AB
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

#include <array>
#include <deque>
#include <list>
#include <set>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

struct sequence_inserter {
  using state = int;
  static const state init_state = 0;

  template <typename container_type, typename value_type>
  static state insert(
      decode_context &,
      state,
      container_type &container,
      value_type &&value) {
    container.push_back(std::forward<value_type>(value));
    return init_state;
  }

  template <typename container_type>
  static void validate(decode_context &, state, container_type &) {
    // Nothing to validate
  }
};

struct fixed_size_sequence_inserter {
  using state = size_t;
  static const state init_state = 0;

  template <typename container_type, typename value_type>
  static state insert(
      decode_context &context,
      state pos,
      container_type &container,
      value_type &&value) {
    fail_if(context, pos >= container.size(), "Too many elements in array");
    container[pos] = value;
    return pos + 1;
  }

  template <typename container_type>
  static void validate(decode_context &context, state pos, container_type &container) {
    fail_if(context, pos != container.size(), "Too few elements in array");
  }
};

struct associative_inserter {
  using state = int;
  static const state init_state = 0;

  template <typename container_type, typename value_type>
  static state insert(
      decode_context &,
      state,
      container_type &container,
      value_type &&value) {
    container.insert(std::forward<value_type>(value));
    return init_state;
  }

  template <typename container_type>
  static void validate(decode_context &, state, container_type &) {
    // Nothing to validate
  }
};

template <typename T> struct container_inserter;

template <typename T>
struct container_inserter<std::vector<T>> : public sequence_inserter {};

template <typename T>
struct container_inserter<std::deque<T>> : public sequence_inserter {};

template <typename T>
struct container_inserter<std::list<T>> : public sequence_inserter {};

template <typename T, size_t Size>
struct container_inserter<std::array<T, Size>> : public fixed_size_sequence_inserter {};

template <typename T>
struct container_inserter<std::set<T>> : public associative_inserter {};

template <typename T>
struct container_inserter<std::unordered_set<T>> : public associative_inserter {};

}  // namespace detail

namespace codec {

template <typename T, typename codec_type>
class array_t final {
 public:
  using object_type = T;

  static_assert(
      std::is_convertible<
          typename T::value_type,
          typename std::decay<codec_type>::type::object_type>::value,
      "Array container type must be convertible to inner codec type");
  static_assert(
      std::is_convertible<
          typename std::decay<codec_type>::type::object_type,
          typename T::value_type>::value,
      "Inner codec type must be convertible to array container type");

  explicit array_t(codec_type inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  object_type decode(decode_context &context) const {
    using inserter = detail::container_inserter<T>;
    object_type output;
    typename inserter::state state = inserter::init_state;
    detail::decode_comma_separated(context, '[', ']', [&]{
      state = inserter::insert(
          context, state, output, _inner_codec.decode(context));
    });
    inserter::validate(context, state, output);
    return output;
  }

  void encode(encode_context &context, const object_type &array) const {
    context.append('[');
    for (const auto &element : array) {
      if (json_likely(detail::should_encode(_inner_codec, element))) {
        _inner_codec.encode(context, element);
        context.append(',');
      }
    }
    context.append_or_replace(',', ']');
  }

 private:
  codec_type _inner_codec;
};

template <typename T, typename codec_type>
array_t<T, typename std::decay<codec_type>::type> array(codec_type &&inner_codec) {
  return array_t<T, typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
}

}  // namespace codec

template <typename T>
struct default_codec_t<std::vector<T>> {
  static decltype(codec::array<std::vector<T>>(default_codec<T>())) codec() {
    return codec::array<std::vector<T>>(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<std::deque<T>> {
  static decltype(codec::array<std::deque<T>>(default_codec<T>())) codec() {
    return codec::array<std::deque<T>>(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<std::list<T>> {
  static decltype(codec::array<std::list<T>>(default_codec<T>())) codec() {
    return codec::array<std::list<T>>(default_codec<T>());
  }
};

template <typename T, size_t size>
struct default_codec_t<std::array<T, size>> {
  static decltype(codec::array<std::array<T, size>>(default_codec<T>())) codec() {
    return codec::array<std::array<T, size>>(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<std::set<T>> {
  static decltype(codec::array<std::set<T>>(default_codec<T>())) codec() {
    return codec::array<std::set<T>>(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<std::unordered_set<T>> {
  static decltype(codec::array<std::unordered_set<T>>(default_codec<T>())) codec() {
    return codec::array<std::unordered_set<T>>(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
