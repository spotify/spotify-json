/*
 * Copyright (c) 2016 Spotify AB
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
#include <utility>

namespace spotify {
namespace json {
namespace detail {

template <typename T, size_t remaining_count, typename... codecs_type>
struct tuple_field final {
  static constexpr size_t element_count = std::tuple_size<T>::value;
  static constexpr size_t element_idx = element_count - remaining_count;

  static void decode(
      const std::tuple<codecs_type...> &codecs,
      decode_context &context,
      T &object) {
    if (element_idx != 0) {
      skip_1(context, ',');
      skip_any_whitespace(context);
    }

    const auto &codec = std::get<element_idx>(codecs);
    std::get<element_idx>(object) = codec.decode(context);
    skip_any_whitespace(context);
    tuple_field<T, remaining_count - 1, codecs_type...>::decode(codecs, context, object);
  }

  static void encode(
      const std::tuple<codecs_type...> &codecs,
      encode_context &context,
      const T &object) {
    const auto &codec = std::get<element_idx>(codecs);
    const auto &element = std::get<element_idx>(object);
    if (json_likely(detail::should_encode(codec, element))) {
      codec.encode(context, element);
      context.append(',');
    }
    tuple_field<T, remaining_count - 1, codecs_type...>::encode(codecs, context, object);
  }
};

template <typename T, typename... codecs_type>
struct tuple_field<T, 0, codecs_type...> {
  static void decode(const std::tuple<codecs_type...> &codecs, decode_context &, T &) {}
  static void encode(const std::tuple<codecs_type...> &codecs, encode_context &, const T &) {}
};

}

namespace codec {

template <typename T, typename... codecs_type>
class tuple_t final {
  static constexpr size_t element_count = std::tuple_size<T>::value;
 public:
  using object_type = T;

  template <typename... Args>
  tuple_t(Args&& ...args) : _codecs(std::forward<Args>(args)...) {}

  object_type decode(decode_context &context) const {
    object_type output;
    detail::skip_1(context, '[');
    detail::skip_any_whitespace(context);
    detail::tuple_field<object_type, element_count, codecs_type...>::decode(
        _codecs, context, output);
    detail::skip_1(context, ']');
    return output;
  }

  void encode(encode_context &context, const object_type &object) const {
    context.append('[');
    detail::tuple_field<object_type, element_count, codecs_type...>::encode(
        _codecs, context, object);
    context.append_or_replace(',', ']');
  }

 private:
  std::tuple<codecs_type ...> _codecs;
};

template <typename... codecs_type>
tuple_t<
    std::tuple<typename std::decay<codecs_type>::type::object_type...>,
    typename std::decay<codecs_type>::type...> tuple(codecs_type&& ...codecs) {
  return tuple_t<
      std::tuple<typename std::decay<codecs_type>::type::object_type...>,
      typename std::decay<codecs_type>::type...>(std::forward<codecs_type>(codecs)...);
}

template <typename codec_type_a, typename codec_type_b>
tuple_t<
    std::pair<typename codec_type_a::object_type, typename codec_type_b::object_type>,
    typename std::decay<codec_type_a>::type,
    typename std::decay<codec_type_b>::type> pair(codec_type_a &&a, codec_type_b &&b) {
  return tuple_t<
    std::pair<typename codec_type_a::object_type, typename codec_type_b::object_type>,
    typename std::decay<codec_type_a>::type,
    typename std::decay<codec_type_b>::type>(
        std::forward<codec_type_a>(a),
        std::forward<codec_type_b>(b));
}

}  // namespace codec

template <typename... T>
struct default_codec_t<std::tuple<T ...>> {
  static decltype(codec::tuple(default_codec<T>()...)) codec() {
    return codec::tuple(default_codec<T>()...);
  }
};

template <typename A, typename B>
struct default_codec_t<std::pair<A, B>> {
  static decltype(codec::pair(default_codec<A>(), default_codec<B>())) codec() {
    return codec::pair(default_codec<A>(), default_codec<B>());
  }
};

}  // namespace json
}  // namespace spotify
