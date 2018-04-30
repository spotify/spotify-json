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

#include <spotify/json/detail/escape.hpp>

#if defined(json_arch_x86_sse42)

#include <nmmintrin.h>

#include "escape_common.hpp"

namespace spotify {
namespace json {
namespace detail {

json_force_inline void write_escaped_16_sse42(char *&out, const __m128i chunk) {
  write_escaped_c(out, _mm_extract_epi8(chunk, 0));
  write_escaped_c(out, _mm_extract_epi8(chunk, 1));
  write_escaped_c(out, _mm_extract_epi8(chunk, 2));
  write_escaped_c(out, _mm_extract_epi8(chunk, 3));
  write_escaped_c(out, _mm_extract_epi8(chunk, 4));
  write_escaped_c(out, _mm_extract_epi8(chunk, 5));
  write_escaped_c(out, _mm_extract_epi8(chunk, 6));
  write_escaped_c(out, _mm_extract_epi8(chunk, 7));
  write_escaped_c(out, _mm_extract_epi8(chunk, 8));
  write_escaped_c(out, _mm_extract_epi8(chunk, 9));
  write_escaped_c(out, _mm_extract_epi8(chunk, 10));
  write_escaped_c(out, _mm_extract_epi8(chunk, 11));
  write_escaped_c(out, _mm_extract_epi8(chunk, 12));
  write_escaped_c(out, _mm_extract_epi8(chunk, 13));
  write_escaped_c(out, _mm_extract_epi8(chunk, 14));
  write_escaped_c(out, _mm_extract_epi8(chunk, 15));
}

void write_escaped_sse42(
    encode_context &context,
    const char *begin,
    const char *end) {
  const auto buf = context.reserve(6 * (end - begin));  // 6 is the length of \u00xx
  auto out = buf;

  const __m128i ranges = _mm_setr_epi8(
    0x00, 0x1F,  // null byte & control characters
    0x22, 0x22,  // double quotation mark
    0x5C, 0x5C,  // reverse solidus (backslash)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  );

  if (json_unaligned_2(begin) && (end - begin) >= 1) { write_escaped_1(out, begin); }
  if (json_unaligned_4(begin) && (end - begin) >= 2) { write_escaped_2(out, begin); }
  if (json_unaligned_8(begin) && (end - begin) >= 4) { write_escaped_4(out, begin); }
  if (json_unaligned_16(begin) && (end - begin) >= 8) { write_escaped_8(out, begin); }

  for (; end - begin >= 16; begin += 16) {
    const __m128i chunk = _mm_load_si128(reinterpret_cast<const __m128i *>(begin));
    const unsigned has_character_in_ranges = _mm_cmpestrc(ranges, 6, chunk, 16, _SIDD_CMP_RANGES);
    if (json_likely(!has_character_in_ranges)) {
      _mm_storeu_si128(reinterpret_cast<__m128i *>(out), chunk);
      out += 16;
    } else {
      write_escaped_16_sse42(out, chunk);
    }
  }

  if ((end - begin) >= 8) { write_escaped_8(out, begin); }
  if ((end - begin) >= 4) { write_escaped_4(out, begin); }
  if ((end - begin) >= 2) { write_escaped_2(out, begin); }
  if ((end - begin) >= 1) { write_escaped_1(out, begin); }

  context.advance(out - buf);
}

}  // namespace detail
}  // namespace json
}  // namespace spotify

#endif  // defined(json_arch_x86_sse42)
