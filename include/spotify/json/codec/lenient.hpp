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
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename InnerCodec>
class lenient_t final {
 public:
  using object_type = typename InnerCodec::object_type;

  lenient_t(InnerCodec inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  void encode(const object_type &value, writer &writer) const {
    _inner_codec.encode(value, writer);
  }

  object_type decode(decoding_context &context) const {
    const char * const original_position = context.position;
    object_type result = _inner_codec.decode(context);
    if (context.has_failed()) {
      context.position = original_position;
      context.error.clear();
      detail::advance_past_value(context);
    }
    return result;
  }

 private:
  InnerCodec _inner_codec;
};

template<typename InnerCodec>
lenient_t<InnerCodec> lenient(InnerCodec &&inner_codec) {
  return lenient_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
