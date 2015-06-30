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

#include <map>
#include <type_traits>
#include <unordered_map>

#include <spotify/json/codec/string.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename T, typename InnerCodec>
class map_t final {
 public:
  using object_type = T;

  static_assert(
      std::is_same<typename T::key_type, std::string>::value,
      "Map key type must be string");
  static_assert(
      std::is_same<
          typename T::mapped_type,
          typename InnerCodec::object_type>::value,
      "Map data type must match inner codec type");

  explicit map_t(InnerCodec inner_codec)
      : _inner_codec(inner_codec) {}

  void encode(const object_type &array, writer &w) const {
    w.add_object([&](writer &w) {
      for (const auto &element : array) {
        w.add_key(element.first);
        _inner_codec.encode(element.second, w);
      }
    });
  }

  object_type decode(decoding_context &context) const {
    using value_type = typename object_type::value_type;
    object_type output;
    const auto string_c = string();
    detail::advance_past_object(
        context,
        [string_c](decoding_context &context) {
          return string_c.decode(context);
        },
        [&](std::string &&key) {
          output.insert(value_type(std::move(key), _inner_codec.decode(context)));
        });
    return output;
  }

 private:
  InnerCodec _inner_codec;
};

template<typename T, typename InnerCodec>
map_t<T, InnerCodec> map(InnerCodec &&inner_codec) {
  return map_t<T, InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec

template<typename T>
struct default_codec_t<std::map<std::string, T>> {
  static decltype(codec::map<std::map<std::string, T>>(default_codec<T>())) codec() {
    return codec::map<std::map<std::string, T>>(default_codec<T>());
  }
};

template<typename T>
struct default_codec_t<std::unordered_map<std::string, T>> {
  static decltype(codec::map<std::unordered_map<std::string, T>>(default_codec<T>())) codec() {
    return codec::map<std::unordered_map<std::string, T>>(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
