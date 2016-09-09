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

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/encoding_helpers.hpp>

namespace spotify {
namespace json {
namespace codec {

/**
 * Codec that requires a specific value in order to parse, or it fails. It
 * always encodes into that specified value.
 *
 * This is useful mainly in "dummy" fields of object codecs that are never
 * saved anywhere, for example to enforce a certain version. It works well
 * together with one_of, which makes it possible to specify different codecs
 * for different versions.
 */
template <typename InnerCodec>
class eq_t final {
 public:
  using object_type = typename InnerCodec::object_type;

  eq_t(InnerCodec inner_codec, object_type value)
      : _inner_codec(std::move(inner_codec)), _value(value) {}

  object_type decode(decoding_context &context) const {
    object_type result = _inner_codec.decode(context);
    detail::fail_if(context, result != _value, "Encountered unexpected value");
    return result;
  }

  void encode(encoding_context &context, const object_type &value) const {
    _inner_codec.encode(context, _value);
  }

  bool should_encode(const object_type &value) const {
    return detail::should_encode(_inner_codec, value);
  }

 private:
  InnerCodec _inner_codec;
  object_type _value;
};

template <typename InnerCodec>
eq_t<typename std::decay<InnerCodec>::type> eq(
    InnerCodec &&inner_codec,
    typename InnerCodec::object_type value) {
  return eq_t<typename std::decay<InnerCodec>::type>(
      std::forward<InnerCodec>(inner_codec),
      std::move(value));
}

template <typename Value>
auto eq(Value &&value) -> decltype(eq(default_codec<Value>(), std::forward<Value>(value))) {
  return eq(default_codec<Value>(), std::forward<Value>(value));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
