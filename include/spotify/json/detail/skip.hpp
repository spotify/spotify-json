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
#include <spotify/json/detail/char_traits.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

#define json_unaligned_x(ignore) true

#define JSON_STRING_SKIP_N_SIMPLE(n, n_plus_one, type, control, goto_label) \
  control ((end - position) >= n && json_unaligned_ ## n_plus_one(position)) { \
    const auto cc = *reinterpret_cast<const type *>(position); \
    if (json_haschar_ ## n(cc, '"')) { goto goto_label; } \
    if (json_haschar_ ## n(cc, '\\')) { goto goto_label; } \
    position += n; \
  }

#if defined(json_arch_x86)
const char *skip_past_simple_characters_sse42(const char *begin, const char *const end);
#endif  // defined(json_arch_x86)

/**
 * Skip past the bytes of the string until either a " or a \ character is
 * found. This method attempts to skip as large chunks of memory as possible
 * at each step, by making sure that the context position is aligned to the
 * appropriate address and then reading and comparing several bytes in a
 * single read operation.
 */
inline void skip_past_simple_characters(decoding_context &context) {
  const auto end = context.end;
  auto position = context.position;

  JSON_STRING_SKIP_N_SIMPLE(1,  2, uint8_t,  if, done_x)
  JSON_STRING_SKIP_N_SIMPLE(2,  4, uint16_t, if, done_2)
  JSON_STRING_SKIP_N_SIMPLE(4,  8, uint32_t, if, done_4)

#if defined(json_arch_x86)
  if (context.has_sse42) {
    JSON_STRING_SKIP_N_SIMPLE(8, 16, uint64_t, if, done_8)
    position = skip_past_simple_characters_sse42(position, end);
  }
#endif  // defined(json_arch_x86)

          JSON_STRING_SKIP_N_SIMPLE(8, x, uint64_t, while, done_8)
  done_8: JSON_STRING_SKIP_N_SIMPLE(4, x, uint32_t, while, done_4)
  done_4: JSON_STRING_SKIP_N_SIMPLE(2, x, uint16_t, while, done_2)
  done_2: JSON_STRING_SKIP_N_SIMPLE(1, x, uint8_t,  while, done_x)
  done_x: context.position = position;
}

#if defined(json_arch_x86)
const char *skip_past_whitespace_sse42(const char *begin, const char *const end);
#endif  // defined(json_arch_x86)

/**
 * Skip past the bytes of the string until a non-whitespace character is
 * found. This method attempts to skip as large chunks of memory as possible
 * at each step, by making sure that the context position is aligned to the
 * appropriate address and then reading and comparing several bytes in a
 * single read operation.
 */
inline void skip_past_whitespace(decoding_context &context) {
  const auto end = context.end;
  auto position = context.position;

#if defined(json_arch_x86)
  if (context.has_sse42) {
    while (position < end &&
           json_unaligned_16(position) &&
           char_traits<char>::is_space(*position)) {
      position++;
    }

    position = skip_past_whitespace_sse42(position, end);
  }
#endif  // defined(json_arch_x86)

  while (position < end &&
         char_traits<char>::is_space(*position)) {
    position++;
  }

  context.position = position;
}

}  // detail
}  // json
}  // spotify
