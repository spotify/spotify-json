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

#include <algorithm>
#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

class string_t final {
 public:
  using object_type = std::string;

  object_type decode(decode_context &context) const;
  void encode(encode_context &context, const object_type value) const;
};

inline string_t string() {
  return string_t();
}

}  // namespace codec

template <>
struct default_codec_t<std::string> {
  static codec::string_t codec() {
    return codec::string_t();
  }
};

}  // namespace json
}  // namespace spotify
