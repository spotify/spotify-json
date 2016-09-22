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

#include <spotify/json/detail/skip.hpp>

#include <spotify/json/detail/char_traits.hpp>
#include <spotify/json/detail/macros.hpp>

#if defined(json_arch_x86)
#include <nmmintrin.h>
#endif

#define json_unaligned_x(ignore) true

#define JSON_STRING_SKIP_N_SIMPLE(n, n_plus_one, type, control, goto_label) \
  control ((end - position) >= n && json_unaligned_ ## n_plus_one(position)) { \
    const auto cc = *reinterpret_cast<const type *>(position); \
    if (json_haschar_ ## n(cc, '"')) { goto goto_label; } \
    if (json_haschar_ ## n(cc, '\\')) { goto goto_label; } \
    position += n; \
  }

namespace spotify {
namespace json {
namespace detail {
namespace {

#if defined(json_arch_x86)

const char *skip_past_simple_characters_sse42(const char *begin, const char *const end) {
  // Determine if and where the next 16 unsigned 8-bit characters contain " or \.
  alignas(16) static const char chars[16] = "\"\\";
  const __m128i a = _mm_load_si128(reinterpret_cast<const __m128i *>(&chars[0]));

  for (; begin <= end - 16; begin += 16) {
    const __m128i b = _mm_load_si128(reinterpret_cast<const __m128i *>(begin));
    const int index = _mm_cmpistri(a, b,
            _SIDD_UBYTE_OPS |
            _SIDD_CMP_EQUAL_ANY |
            _SIDD_POSITIVE_POLARITY |
            _SIDD_LEAST_SIGNIFICANT);
    if (index != 16) {
      begin += index;
      break;
    }
  }

  return begin;
}

const char *skip_past_whitespace_sse42(const char *begin, const char *const end) {
  // Determine if and where the next 16 unsigned 8-bit characters contain whitespace.
  alignas(16) static const char chars[16] = " \t\n\r";
  const __m128i a = _mm_load_si128(reinterpret_cast<const __m128i *>(&chars[0]));

  for (; begin <= end - 16; begin += 16) {
    const __m128i b = _mm_load_si128(reinterpret_cast<const __m128i *>(begin));
    const int index = _mm_cmpistri(a, b,
            _SIDD_UBYTE_OPS |
            _SIDD_CMP_EQUAL_ANY |
            _SIDD_NEGATIVE_POLARITY |
            _SIDD_LEAST_SIGNIFICANT);
    if (index != 16) {
      begin += index;
      break;
    }
  }

  return begin;
}

#endif  // defined(json_arch_x86)

}  // namespace

void skip_past_simple_characters(decode_context &context) {
  const auto end = context.end;
  auto position = context.position;

  JSON_STRING_SKIP_N_SIMPLE(1,  2, uint8_t,  if, done_x)
  JSON_STRING_SKIP_N_SIMPLE(2,  4, uint16_t, if, done_2)
  JSON_STRING_SKIP_N_SIMPLE(4,  8, uint32_t, if, done_4)

#if defined(json_arch_x86)
  if (json_likely(context.has_sse42)) {
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

void skip_past_whitespace(decode_context &context) {
  const auto end = context.end;
  auto pos = context.position;

#if defined(json_arch_x86)
  if (json_likely(context.has_sse42)) {
    for (; pos < end && json_unaligned_16(pos); ++pos) {
      if (!char_traits<char>::is_space(*pos)) {
        context.position = pos;
        return;
      }
    }

    pos = skip_past_whitespace_sse42(pos, end);
  }
#endif  // defined(json_arch_x86)

  while (pos < end && char_traits<char>::is_space(*pos)) {
    ++pos;
  }

  context.position = pos;
}

}  // detail
}  // json
}  // spotify
