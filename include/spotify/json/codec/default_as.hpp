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
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename DefaultCodec, typename InnerCodec>
class default_as_t final {
 public:
  static_assert(
      std::is_same<
          typename InnerCodec::object_type,
          typename InnerCodec::object_type>::value,
      "The codecs provided to default_as_t must encode the same type");

  using object_type = typename InnerCodec::object_type;

  default_as_t() = default;

  default_as_t(DefaultCodec default_codec, InnerCodec inner_codec)
      : _default_codec(std::move(default_codec)),
        _inner_codec(std::move(inner_codec)) {}

  void encode(const object_type &value, detail::writer &w) const {
    if (value == _default) {
      _default_codec.encode(value, w);
    } else {
      _inner_codec.encode(value, w);
    }
  }

  object_type decode(decoding_context &context) const {
    const auto original_position = context.position;
    try {
      return _inner_codec.decode(context);
    } catch (const decode_exception &exc) {
      context.position = original_position;
      try {
        return _default_codec.decode(context);
      } catch (const decode_exception &) {
        // The error of the inner codec is more interested than saying for
        // example that the object is not a valid null.
        throw exc;
      }
    }
  }

  bool should_encode(const object_type &value) const {
    if (value == _default) {
      return detail::should_encode(_default_codec, value);
    } else {
      return detail::should_encode(_inner_codec, value);
    }
  }

 private:
  DefaultCodec _default_codec;
  InnerCodec _inner_codec;
  object_type _default = object_type();
};

template <typename DefaultCodec, typename InnerCodec>
default_as_t<typename std::decay<DefaultCodec>::type, typename std::decay<InnerCodec>::type> default_as(
    DefaultCodec &&default_codec,
    InnerCodec &&inner_codec) {
  return default_as_t<typename std::decay<DefaultCodec>::type, typename std::decay<InnerCodec>::type>(
      std::forward<DefaultCodec>(default_codec),
      std::forward<InnerCodec>(inner_codec));
}

template <typename InnerCodec>
default_as_t<
    null_t<typename std::decay<InnerCodec>::type::object_type>,
    typename std::decay<InnerCodec>::type> default_as_null(
        InnerCodec &&inner_codec) {
  return default_as(
      null<typename std::decay<InnerCodec>::type::object_type>(),
      std::forward<InnerCodec>(inner_codec));
}

template <typename InnerCodec>
default_as_t<
    omit_t<typename std::decay<InnerCodec>::type::object_type>,
    typename std::decay<InnerCodec>::type> default_as_omit(
        InnerCodec &&inner_codec) {
  return default_as(
      omit<typename std::decay<InnerCodec>::type::object_type>(),
      std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
