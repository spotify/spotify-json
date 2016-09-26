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

#include <memory>
#include <utility>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename to_ptr_type, typename from_ptr_type>
struct codec_cast;

template <typename T, typename F>
struct codec_cast<std::shared_ptr<T>, std::shared_ptr<F>> {
  static std::shared_ptr<T> cast(const std::shared_ptr<F> &ptr) {
    return std::dynamic_pointer_cast<T>(ptr);
  }
};

template <typename T, typename codec_type>
class cast_t {
 public:
  using object_type = T;

  explicit cast_t(codec_type inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  object_type decode(decode_context &context) const {
    return _inner_codec.decode(context);
  }

  void encode(encode_context &context, object_type value) const {
    using inner_type = typename codec_type::object_type;
    _inner_codec.encode(context, codec_cast<inner_type, T>::cast(value));
  }

 private:
  codec_type _inner_codec;
};

template <typename OuterType, typename codec_type>
cast_t<OuterType, typename std::decay<codec_type>::type> cast(codec_type &&inner_codec) {
  return cast_t<OuterType, typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
