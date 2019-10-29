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

#include <map>
#include <type_traits>
#include <unordered_map>

#include <spotify/json/codec/string.hpp>
#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T, typename codec_type>
class map_t final {
 public:
  using object_type = T;

  static_assert(
      std::is_same<typename T::key_type, std::string>::value,
      "Map key type must be string");
  static_assert(
      std::is_convertible<
          typename T::mapped_type,
          typename codec_type::object_type>::value,
      "Map data type must be convertible to inner codec type");
  static_assert(
      std::is_convertible<
          typename codec_type::object_type,
          typename T::mapped_type>::value,
      "Inner codec type must be convertible to map data type");

  explicit map_t(codec_type &&inner_codec) : _inner_codec(std::move(inner_codec)) {}
  explicit map_t(const codec_type &inner_codec) : _inner_codec(inner_codec) {}

  object_type decode(decode_context &context) const {
    using value_type = typename object_type::value_type;
    object_type output;
    detail::decode_object<string_t>(
        context,
        [&](std::string &&key) {
          output.insert(value_type(std::move(key), _inner_codec.decode(context)));
        });
    return output;
  }

  void encode(encode_context &context, const object_type &map) const {
    context.append('{');
    for (const auto &element : map) {
      if (json_likely(detail::should_encode(_inner_codec, element.second))) {
        _string_codec.encode(context, element.first);
        context.append(':');
        _inner_codec.encode(context, element.second);
        context.append(',');
      }
    }
    context.append_or_replace(',', '}');
  }

 private:
  string_t _string_codec;
  codec_type _inner_codec;
};

template <typename T, typename codec_type>
map_t<T, typename std::decay<codec_type>::type> map(codec_type &&inner_codec) {
  return map_t<T, typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
}

}  // namespace codec

template <typename T>
struct default_codec_t<std::map<std::string, T>> {
  static decltype(codec::map<std::map<std::string, T>>(default_codec<T>())) codec() {
    return codec::map<std::map<std::string, T>>(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<std::unordered_map<std::string, T>> {
  static decltype(codec::map<std::unordered_map<std::string, T>>(default_codec<T>())) codec() {
    return codec::map<std::unordered_map<std::string, T>>(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
