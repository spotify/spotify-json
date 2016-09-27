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

#include <spotify/json/decode_context.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename... codecs_type>
struct codecs_share_same_object_type;

template <typename codec_type>
struct codecs_share_same_object_type<codec_type> : std::true_type {};

template <typename codec_type_1, typename codec_type_2, typename... codecs_type>
struct codecs_share_same_object_type<codec_type_1, codec_type_2, codecs_type...>
    : std::integral_constant<
          bool,
          std::is_same<
              typename codec_type_1::object_type,
              typename codec_type_2::object_type>::value &&
          codecs_share_same_object_type<codec_type_2, codecs_type...>::value> {};

template <typename tuple_type, size_t N>
struct try_each_codec {
  using object_type = typename std::tuple_element<
      std::tuple_size<tuple_type>::value - N, tuple_type>::type::object_type;

  static object_type decode(const tuple_type &tuple, decode_context &context) {
    const auto original_position = context.position;
    try {
      return std::get<std::tuple_size<tuple_type>::value - N>(tuple).decode(context);
    } catch (const decode_exception &) {
      context.position = original_position;
      return try_each_codec<tuple_type, N - 1>::decode(tuple, context);
    }
  }
};

template <typename tuple_type>
struct try_each_codec<tuple_type, 0> {
  using object_type = typename std::tuple_element<0, tuple_type>::type::object_type;

  static object_type decode(const tuple_type &tuple, decode_context &context) {
    return std::get<std::tuple_size<tuple_type>::value - 1>(tuple).decode(context);
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
template <typename codec_type, typename... codecs_type>
class one_of_t final {
 public:
  static_assert(
      detail::codecs_share_same_object_type<codec_type, codecs_type ...>::value,
      "All of the provided codecs to one_of_t must encode the same type");

  using object_type = typename codec_type::object_type;

  template <typename... Args>
  explicit one_of_t(Args&& ...args)
      : _codecs(std::forward<Args>(args)...) {}

  object_type decode(decode_context &context) const {
    return detail::try_each_codec<
        decltype(_codecs), std::tuple_size<decltype(_codecs)>::value>::decode(_codecs, context);
  }

  void encode(encode_context &context, const object_type &value) const {
    std::get<0>(_codecs).encode(context, value);
  }

  bool should_encode(const object_type &value) const {
    return detail::should_encode(std::get<0>(_codecs), value);
  }

 private:
  std::tuple<codec_type, codecs_type ...> _codecs;
};

template <typename... codecs_type>
one_of_t<typename std::decay<codecs_type>::type...> one_of(codecs_type&& ...codecs) {
  return one_of_t<typename std::decay<codecs_type>::type...>(std::forward<codecs_type>(codecs)...);
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
