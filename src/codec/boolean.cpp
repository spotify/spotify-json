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

#include <spotify/json/codec/boolean.hpp>

#include <cstring>
#include <spotify/json/decode_exception.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/skip_chars.hpp>

#if _MSC_VER
#pragma intrinsic (memcpy)
#endif

namespace spotify {
namespace json {
namespace codec {

boolean_t::object_type boolean_t::decode(decode_context &context) const {
  switch (detail::peek(context)) {
    case 'f': detail::skip_false(context); return false;
    case 't': detail::skip_true(context); return true;
    default: detail::fail(context, "Unexpected input, expected boolean");
  }
}

void boolean_t::encode(encode_context &context, const object_type value) const {
  const auto needed = 5 - size_t(value);  // true: 4, false: 5
  const auto buffer = context.reserve(needed);
  memcpy(buffer, value ? "true" : "fals", 4);  // 4 byte writes optimize well on x86
  buffer[needed - 1] = 'e'; // write the missing 'e' in 'false' (or overwrite it in 'true')
  context.advance(needed);
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
