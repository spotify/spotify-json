/*
 * Copyright (c) 2015-2019 Spotify AB
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

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

class boolean_t final {
 public:
  using object_type = bool;

  object_type decode(decode_context &context) const;
  void encode(encode_context &context, const object_type value) const;
};

inline boolean_t boolean() {
  return boolean_t();
}

}  // namespace codec

template <>
struct default_codec_t<bool> {
  static codec::boolean_t codec() {
    return codec::boolean_t();
  }
};

}  // namespace json
}  // namespace spotify
