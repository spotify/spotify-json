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

#if defined(json_arch_x86)

#include <nmmintrin.h>

namespace spotify {
namespace json {
namespace detail {

json_force_inline unsigned long json_ffs(unsigned x) {
#if defined(_MSC_VER)
  unsigned long offset;
  _BitScanForward(&offset, x);
  return offset;
#elif defined(__GNUC__)
  return (__builtin_ffs(x) - 1);
#endif  // defined(_MSC_VER)
}

const char *skip_past_simple_characters_sse42(const char *begin, const char *const end) {
  // Determine if the next 16 unsigned 8-bit characters contain " or \.
  alignas(16) static const char chars[16] = "\"\\";
  const __m128i a = _mm_load_si128(reinterpret_cast<const __m128i *>(&chars[0]));

  for (; begin <= end - 16; begin += 16) {
    const __m128i b = _mm_load_si128(reinterpret_cast<const __m128i *>(begin));
    const unsigned result = _mm_cvtsi128_si32(
        _mm_cmpistrm(a, b,
            _SIDD_UBYTE_OPS |
            _SIDD_CMP_EQUAL_ANY |
            _SIDD_POSITIVE_POLARITY |
            _SIDD_BIT_MASK));
    // Find index of first " or \ character.
    if (result != 0) {
      begin += json_ffs(result);
      break;
    }
  }

  return begin;
}

const char *skip_past_whitespace_sse42(const char *begin, const char *const end) {
  // Determine if the next 16 unsigned 8-bit characters contain whitespace characters.
  alignas(16) static const char chars[16] = " \t\n\r";
  const __m128i a = _mm_load_si128(reinterpret_cast<const __m128i *>(&chars[0]));

  for (; begin <= end - 16; begin += 16) {
    const __m128i b = _mm_load_si128(reinterpret_cast<const __m128i *>(begin));
    const unsigned result = _mm_cvtsi128_si32(
        _mm_cmpistrm(a, b,
            _SIDD_UBYTE_OPS |
            _SIDD_CMP_EQUAL_ANY |
            _SIDD_NEGATIVE_POLARITY |
            _SIDD_BIT_MASK));
    // Find index of first non-whitespace character.
    if (result != 0) {
      begin += json_ffs(result);
      break;
    }
  }

  return begin;
}

}  // detail
}  // json
}  // spotify

#endif  // defined(json_arch_x86)
