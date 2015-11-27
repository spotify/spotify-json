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

#pragma once

#include <deque>
#include <list>
#include <set>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace detail {

template<typename Container>
struct SequenceInserter {
  static void apply(
      Container &container, typename Container::value_type &&value) {
    container.push_back(std::forward<typename Container::value_type>(value));
  }
};

template<typename Container>
struct AssociativeInserter {
  static void apply(
      Container &container, typename Container::value_type &&value) {
    container.insert(std::forward<typename Container::value_type>(value));
  }
};

}  // namespace detail

namespace codec {

template<typename T, typename InnerCodec, typename Inserter>
class array_t final {
 public:
  using object_type = T;

  static_assert(
      std::is_same<
          typename T::value_type,
          typename std::decay<InnerCodec>::type::object_type>::value,
      "Array container type must match inner codec type");

  explicit array_t(InnerCodec inner_codec)
      : _inner_codec(inner_codec) {}

  void encode(const object_type &array, writer &w) const {
    w.add_array([&](writer &w) {
      for (const auto &element : array) {
        _inner_codec.encode(element, w);
      }
    });
  }

  object_type decode(decoding_context &context) const {
    object_type output;
    detail::advance_past_comma_separated(context, '[', ']', [&]{
      Inserter::apply(output, _inner_codec.decode(context));
    });
    return output;
  }

 private:
  InnerCodec _inner_codec;
};

template<typename T, typename InnerCodec>
array_t<T, InnerCodec, detail::SequenceInserter<T>> array(InnerCodec &&inner_codec) {
  return array_t<T, InnerCodec, detail::SequenceInserter<T>>(
      std::forward<InnerCodec>(inner_codec));
}

template<typename T, typename InnerCodec>
array_t<T, InnerCodec, detail::AssociativeInserter<T>> set(InnerCodec &&inner_codec) {
  return array_t<T, InnerCodec, detail::AssociativeInserter<T>>(
      std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec

template<typename T>
struct default_codec_t<std::vector<T>> {
  static decltype(codec::array<std::vector<T>>(default_codec<T>())) codec() {
    return codec::array<std::vector<T>>(default_codec<T>());
  }
};

template<typename T>
struct default_codec_t<std::deque<T>> {
  static decltype(codec::array<std::deque<T>>(default_codec<T>())) codec() {
    return codec::array<std::deque<T>>(default_codec<T>());
  }
};

template<typename T>
struct default_codec_t<std::list<T>> {
  static decltype(codec::array<std::list<T>>(default_codec<T>())) codec() {
    return codec::array<std::list<T>>(default_codec<T>());
  }
};

template<typename T>
struct default_codec_t<std::set<T>> {
  static decltype(codec::set<std::set<T>>(default_codec<T>())) codec() {
    return codec::set<std::set<T>>(default_codec<T>());
  }
};

template<typename T>
struct default_codec_t<std::unordered_set<T>> {
  static decltype(codec::set<std::unordered_set<T>>(default_codec<T>())) codec() {
    return codec::set<std::unordered_set<T>>(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
