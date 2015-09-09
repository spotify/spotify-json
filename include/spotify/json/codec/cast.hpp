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

#include <memory>
#include <utility>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename ToPointerType, typename FromPointerType>
struct codec_cast;

template<typename ToType, typename FromType>
struct codec_cast<std::shared_ptr<ToType>, std::shared_ptr<FromType>> {
  static std::shared_ptr<ToType> cast(const std::shared_ptr<FromType> &ptr) {
    return std::dynamic_pointer_cast<ToType>(ptr);
  }
};

template<typename PointerType, typename InnerCodec>
class cast_t {
 public:
  using object_type = PointerType;

  explicit cast_t(InnerCodec inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  void encode(object_type value, writer &w) const {
    _inner_codec.encode(
        codec_cast<typename InnerCodec::object_type, PointerType>::cast(value), w);
  }

  object_type decode(decoding_context &context) const {
    return _inner_codec.decode(context);
  }

 private:
  InnerCodec _inner_codec;
};

template<typename OuterType, typename InnerCodec>
cast_t<OuterType, InnerCodec> cast(InnerCodec &&inner_codec) {
  return cast_t<OuterType, InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
