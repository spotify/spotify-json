/*
 * Copyright (c) 2014-2019 Spotify AB
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

#include <optional>
#include <type_traits>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>

namespace spotify {
namespace json {
namespace codec {

template <
    typename codec_type,
    typename optional_type = std::optional<typename codec_type::object_type>,
    typename nullopt_type = std::nullopt_t>
class optional_t {
 public:
  using object_type = optional_type;

  explicit optional_t(codec_type &&inner_codec) : _inner_codec(std::move(inner_codec)) {}
  explicit optional_t(const codec_type &inner_codec) : _inner_codec(inner_codec) {}

  object_type decode(decode_context &context) const {
    return _inner_codec.decode(context);
  }

  template <typename value_type>
  void encode(encode_context &context, const value_type &value) const {
    detail::fail_if(context, !value, "Cannot encode null optional");
    _inner_codec.encode(context, *value);
  }

  template <typename value_type>
  bool should_encode(const value_type &value) const {
    return value && detail::should_encode(_inner_codec, *value);
  }

  bool should_encode(const nullopt_type &) const {
    return false;
  }

 private:
  codec_type _inner_codec;
};

template <typename codec_type, typename... options_type>
optional_t<typename std::decay<codec_type>::type> optional(codec_type &&inner_codec, options_type... options) {
  return optional_t<typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec), options...);
}

}  // namespace codec

template <typename T>
struct default_codec_t<std::optional<T>> {
  static decltype(codec::optional(default_codec<T>())) codec() {
    return codec::optional(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
