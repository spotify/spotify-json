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

#include <tuple>
#include <type_traits>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename... Codecs>
struct codecs_share_same_object_type;

template <typename Codec>
struct codecs_share_same_object_type<Codec> : std::true_type {};

template <typename FirstCodec, typename SecondCodec, typename... Codecs>
struct codecs_share_same_object_type<FirstCodec, SecondCodec, Codecs...>
    : std::integral_constant<
          bool,
          std::is_same<
              typename FirstCodec::object_type,
              typename SecondCodec::object_type>::value &&
          codecs_share_same_object_type<SecondCodec, Codecs...>::value> {};

template <typename Tuple, size_t N>
struct try_each_codec {
  using object_type = typename std::tuple_element<
      std::tuple_size<Tuple>::value - N, Tuple>::type::object_type;

  static object_type decode(const Tuple &tuple, decoding_context &context) {
    const auto original_position = context.position;
    try {
      return std::get<std::tuple_size<Tuple>::value - N>(tuple).decode(context);
    } catch (const decode_exception &) {
      context.position = original_position;
      return try_each_codec<Tuple, N-1>::decode(tuple, context);
    }
  }
};

template <typename Tuple>
struct try_each_codec<Tuple, 0> {
  using object_type = typename std::tuple_element<0, Tuple>::type::object_type;

  static object_type decode(const Tuple &tuple, decoding_context &context) {
    return std::get<std::tuple_size<Tuple>::value - 1>(tuple).decode(context);
  }
};

}  // namespace detail

namespace codec {

/**
 * Takes an ordered list of codecs and applies them one by one. The first
 * one that succeeds will be used.
 *
 * When encoding, the first codec is always used.
 */
template <typename Codec, typename... Codecs>
class one_of_t final {
 public:
  static_assert(
      detail::codecs_share_same_object_type<Codec, Codecs ...>::value,
      "All of the provided codecs to one_of_t must encode the same type");

  using object_type = typename Codec::object_type;

  template <typename... Args>
  explicit one_of_t(Args&& ...args)
      : _codecs(std::forward<Args>(args)...) {}

  object_type decode(decoding_context &context) const {
    return detail::try_each_codec<
        decltype(_codecs), std::tuple_size<decltype(_codecs)>::value>::decode(_codecs, context);
  }

  void encode(encoding_context &context, const object_type &value) const {
    std::get<0>(_codecs).encode(context, value);
  }

  bool should_encode(const object_type &value) const {
    return detail::should_encode(std::get<0>(_codecs), value);
  }

 private:
  std::tuple<Codec, Codecs ...> _codecs;
};

template <typename... Codecs>
one_of_t<typename std::decay<Codecs>::type...> one_of(Codecs&& ...codecs) {
  return one_of_t<typename std::decay<Codecs>::type...>(std::forward<Codecs>(codecs)...);
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
