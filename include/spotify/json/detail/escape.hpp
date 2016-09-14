/*
 * Copyright (c) 2014-2016 Spotify AB
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

#include <cstdint>
#include <string>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename OutputType>
struct escape_traits {
  static void put(OutputType &out, char c) {
    out.put(c);
  }
};

template <>
json_force_inline void escape_traits<std::string>::put(std::string &out, const char c) {
  out.push_back(c);
}

template <>
json_force_inline void escape_traits<uint8_t *>::put(uint8_t *&out, const char c) {
  *(out++) = c;
}

template <typename OutputType>
json_force_inline void write_escaped_c(OutputType &out, const uint8_t c) {
  using traits = escape_traits<OutputType>;

  static const char *HEX = "0123456789ABCDEF";
  static const char POPULAR_CONTROL_CHARACTERS[] = {
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u',
    'b', 't', 'n', 'u', 'f', 'r', 'u', 'u',
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u',
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u'
  };

  if (json_unlikely(c == '\\' || c == '"' || c == '/')) {
    traits::put(out, '\\');
    traits::put(out, c);
    return;
  }

  if (json_likely(c >= 0x20)) {
    traits::put(out, c);
    return;
  }

  const auto control_character = POPULAR_CONTROL_CHARACTERS[c];
  traits::put(out, '\\');
  traits::put(out, control_character);

  if (json_unlikely(control_character == 'u')) {
    traits::put(out, '0');
    traits::put(out, '0');
    traits::put(out, HEX[(c >> 4)]);
    traits::put(out, HEX[(c & 0x0F)]);
  }
}

template <typename OutputType>
json_force_inline void write_escaped_1(OutputType &out, const uint8_t *&begin) {
  struct blob_1_t { uint8_t a; };
  const auto b = *reinterpret_cast<const blob_1_t *>(begin);
  write_escaped_c(out, b.a);
  begin += sizeof(blob_1_t);
}

template <typename OutputType>
json_force_inline void write_escaped_2(OutputType &out, const uint8_t *&begin) {
  struct blob_2_t { uint8_t a, b; };
  const auto b = *reinterpret_cast<const blob_2_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  begin += sizeof(blob_2_t);
}

template <typename OutputType>
json_force_inline void write_escaped_4(OutputType &out, const uint8_t *&begin) {
  struct blob_4_t { uint8_t a, b, c, d; };
  const auto b = *reinterpret_cast<const blob_4_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  write_escaped_c(out, b.c);
  write_escaped_c(out, b.d);
  begin += sizeof(blob_4_t);
}

template <typename OutputType>
json_force_inline void write_escaped_8(OutputType &out, const uint8_t *&begin) {
  struct blob_8_t { uint8_t a, b, c, d, e, f, g, h; };
  const auto b = *reinterpret_cast<const blob_8_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  write_escaped_c(out, b.c);
  write_escaped_c(out, b.d);
  write_escaped_c(out, b.e);
  write_escaped_c(out, b.f);
  write_escaped_c(out, b.g);
  write_escaped_c(out, b.h);
  begin += sizeof(blob_8_t);
}

/**
  * \brief Escape a string for use in a JSON string as per RFC 4627.
  *
  * This escapes control characters (0x00 through 0x1F), as well as
  * backslashes and quotation marks.
  *
  * See: http://www.ietf.org/rfc/rfc4627.txt (Section 2.5)
  */
template <typename OutputType>
json_never_inline inline void write_escaped(
    OutputType &out,
    const uint8_t *begin,
    const uint8_t *end) {
  if (json_unaligned_2(begin) && (end - begin) >= 1) { write_escaped_1(out, begin); }
  if (json_unaligned_4(begin) && (end - begin) >= 2) { write_escaped_2(out, begin); }
  if (json_unaligned_8(begin) && (end - begin) >= 4) { write_escaped_4(out, begin); }
  while ((end - begin) >= 8) { write_escaped_8(out, begin); }
  if ((end - begin) >= 4) { write_escaped_4(out, begin); }
  if ((end - begin) >= 2) { write_escaped_2(out, begin); }
  if ((end - begin) >= 1) { write_escaped_1(out, begin); }
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
