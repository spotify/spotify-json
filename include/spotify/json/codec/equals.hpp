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

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/writer.hpp>

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
template<typename InnerCodec>
class equals_t final {
 public:
  using object_type = typename InnerCodec::object_type;

  equals_t(InnerCodec inner_codec, object_type value)
      : _inner_codec(std::move(inner_codec)), _value(value) {}

  void encode(const object_type &value, writer &writer) const {
    _inner_codec.encode(_value, writer);
  }

  object_type decode(decoding_context &context) const {
    object_type result = _inner_codec.decode(context);
    detail::require(context, result == _value, "Encountered unexpected value");
    return result;
  }

 private:
  InnerCodec _inner_codec;
  object_type _value;
};

template<typename InnerCodec>
equals_t<InnerCodec> equals(InnerCodec &&inner_codec, typename InnerCodec::object_type value) {
  return equals_t<InnerCodec>(std::forward<InnerCodec>(inner_codec), std::move(value));
}

template<typename Value>
auto equals(Value &&value) -> decltype(equals(default_codec<Value>(), std::forward<Value>(value))) {
  return equals(default_codec<Value>(), std::forward<Value>(value));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
