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
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {

struct null_type {};
static null_type null;

namespace codec {

template <typename ObjectType>
class null_t final {
 public:
  using object_type = ObjectType;

  object_type decode(decoding_context &context) const {
    detail::advance_past_null(context);
    return object_type();
  }

  void encode(encoding_context &context, const object_type value) const {
    context.append("null", 4);
  }
};

template <typename ObjectType = null_type>
inline null_t<ObjectType> null() {
  return null_t<ObjectType>();
}

}  // namespace codec

template <>
struct default_codec_t<null_type> {
  static codec::null_t<null_type> codec() {
    return codec::null_t<null_type>();
  }
};

}  // namespace json
}  // namespace spotify
