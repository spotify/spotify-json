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

template <typename T, size_t RemainingObjects, typename... Codecs>
struct tuple_field {
  static constexpr size_t element_count = std::tuple_size<T>::value;
  static constexpr size_t element_idx = element_count - RemainingObjects;

  static void decode(
      const std::tuple<Codecs...> &codecs,
      decode_context &context,
      T &object) {
    if (element_idx != 0) {
      skip_1(context, ',');
      skip_any_whitespace(context);
    }

    const auto &codec = std::get<element_idx>(codecs);
    std::get<element_idx>(object) = codec.decode(context);
    skip_any_whitespace(context);
    tuple_field<T, RemainingObjects - 1, Codecs...>::decode(codecs, context, object);
  }

  static void encode(
      const std::tuple<Codecs...> &codecs,
      encode_context &context,
      const T &object) {
    const auto &codec = std::get<element_idx>(codecs);
    const auto &element = std::get<element_idx>(object);
    if (json_likely(detail::should_encode(codec, element))) {
      codec.encode(context, element);
      context.append(',');
    }
    tuple_field<T, RemainingObjects - 1, Codecs...>::encode(codecs, context, object);
  }
};

template <typename T, typename... Codecs>
struct tuple_field<T, 0, Codecs...> {
  static void decode(
      const std::tuple<Codecs...> &codecs, decode_context &, T &) {}
  static void encode(
      const std::tuple<Codecs...> &codecs, encode_context &, const T &) {}
};

}

namespace codec {

template <typename T, typename... Codecs>
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
    detail::tuple_field<object_type, element_count, Codecs...>::decode(
        _codecs, context, output);
    detail::skip_1(context, ']');
    return output;
  }

  void encode(encode_context &context, const object_type &object) const {
    context.append('[');
    detail::tuple_field<object_type, element_count, Codecs...>::encode(
        _codecs, context, object);
    context.append_or_replace(',', ']');
  }

 private:
  std::tuple<Codecs ...> _codecs;
};

template <typename... Codecs>
tuple_t<
    std::tuple<typename std::decay<Codecs>::type::object_type...>,
    typename std::decay<Codecs>::type...> tuple(Codecs&& ...codecs) {
  return tuple_t<
      std::tuple<typename std::decay<Codecs>::type::object_type...>,
      typename std::decay<Codecs>::type...>(std::forward<Codecs>(codecs)...);
}

template <typename CodecA, typename CodecB>
tuple_t<
    std::pair<typename CodecA::object_type, typename CodecB::object_type>,
    typename std::decay<CodecA>::type,
    typename std::decay<CodecB>::type> pair(CodecA &&a, CodecB &&b) {
  return tuple_t<
    std::pair<typename CodecA::object_type, typename CodecB::object_type>,
    typename std::decay<CodecA>::type,
    typename std::decay<CodecB>::type>(
        std::forward<CodecA>(a),
        std::forward<CodecB>(b));
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
