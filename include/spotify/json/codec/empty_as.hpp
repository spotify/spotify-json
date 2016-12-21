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

#include <type_traits>

#include <spotify/json/codec/null.hpp>
#include <spotify/json/codec/omit.hpp>
#include <spotify/json/decode_context.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename empty_codec_type, typename inner_codec_type>
class empty_as_t final {
 public:
  static_assert(
      std::is_same<
          typename empty_codec_type::object_type,
          typename inner_codec_type::object_type>::value,
      "The codecs provided to empty_as_t must encode the same type");

  using object_type = typename inner_codec_type::object_type;

  empty_as_t() = default;

  empty_as_t(empty_codec_type empty_codec, inner_codec_type inner_codec)
      : _empty_codec(std::move(empty_codec)),
        _inner_codec(std::move(inner_codec)) {}

  object_type decode(decode_context &context) const {
    const auto original_position = context.position;
    try {
      return _inner_codec.decode(context);
    } catch (const decode_exception &exception) {
      try {
        context.position = original_position;
        return _empty_codec.decode(context);
      } catch (const decode_exception &) {
        // The error of the inner codec is more interesting than saying, for
        // example, that the object is not a valid null.
        throw exception;
      }
    }
  }

  void encode(encode_context &context, const object_type &value) const {
    if (value == _default) {
      _empty_codec.encode(context, value);
    } else {
      _inner_codec.encode(context, value);
    }
  }

  bool should_encode(const object_type &value) const {
    if (value == _default) {
      return detail::should_encode(_empty_codec, value);
    } else {
      return detail::should_encode(_inner_codec, value);
    }
  }

 private:
  empty_codec_type _empty_codec;
  inner_codec_type _inner_codec;
  object_type _default = object_type();
};

template <typename empty_codec_type, typename inner_codec_type>
empty_as_t<typename std::decay<empty_codec_type>::type, typename std::decay<inner_codec_type>::type> empty_as(
    empty_codec_type &&empty_codec,
    inner_codec_type &&inner_codec) {
  return empty_as_t<typename std::decay<empty_codec_type>::type, typename std::decay<inner_codec_type>::type>(
      std::forward<empty_codec_type>(empty_codec),
      std::forward<inner_codec_type>(inner_codec));
}

template <typename inner_codec_type>
empty_as_t<
    null_t<typename std::decay<inner_codec_type>::type::object_type>,
    typename std::decay<inner_codec_type>::type> empty_as_null(inner_codec_type &&inner_codec) {
  return empty_as(
      null<typename std::decay<inner_codec_type>::type::object_type>(),
      std::forward<inner_codec_type>(inner_codec));
}

template <typename inner_codec_type>
empty_as_t<
    omit_t<typename std::decay<inner_codec_type>::type::object_type>,
    typename std::decay<inner_codec_type>::type> empty_as_omit(inner_codec_type &&inner_codec) {
  return empty_as(
      omit<typename std::decay<inner_codec_type>::type::object_type>(),
      std::forward<inner_codec_type>(inner_codec));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
