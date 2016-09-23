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

#include <spotify/json/decode_context.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

void skip_past_simple_characters_scalar(decode_context &context);
#if defined(json_arch_x86)
void skip_past_simple_characters_sse42(decode_context &context);
#endif  // defined(json_arch_x86)

/**
 * Skip past the bytes of the string until either a " or a \ character is
 * found. This method attempts to skip as large chunks of memory as possible
 * at each step, by making sure that the context position is aligned to the
 * appropriate address and then reading and comparing several bytes in a
 * single read operation.
 */
json_force_inline void skip_past_simple_characters(decode_context &context) {
#if defined(json_arch_x86)
  if (json_likely(context.has_sse42)) {
    return skip_past_simple_characters_sse42(context);
  }
#endif  // defined(json_arch_x86)
  return skip_past_simple_characters_scalar(context);
}

void skip_past_whitespace_scalar(decode_context &context);
#if defined(json_arch_x86)
void skip_past_whitespace_sse42(decode_context &context);
#endif  // defined(json_arch_x86)

/**
 * Skip past the bytes of the string until a non-whitespace character is
 * found. This method attempts to skip as large chunks of memory as possible
 * at each step, by making sure that the context position is aligned to the
 * appropriate address and then reading and comparing several bytes in a
 * single read operation.
 */
json_force_inline void skip_past_whitespace(decode_context &context) {
#if defined(json_arch_x86)
  if (json_likely(context.has_sse42)) {
    return skip_past_whitespace_sse42(context);
  }
#endif  // defined(json_arch_x86)
  skip_past_whitespace_scalar(context);
}

}  // detail
}  // json
}  // spotify
