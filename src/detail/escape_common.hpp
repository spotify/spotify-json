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
#include <cstring>

#include <spotify/json/detail/macros.hpp>

#if _MSC_VER
#pragma intrinsic (memcpy)
#endif

namespace spotify {
namespace json {
namespace detail {

json_force_inline void write_escaped_c(char *&out, const char c) {
  static const char HEX[] = "0123456789ABCDEF";
  static const char POPULAR_CONTROL_CHARACTERS[] = {
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u',
    'b', 't', 'n', 'u', 'f', 'r', 'u', 'u',
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u',
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u'
  };

  // All characters above 0x20 can be written as is, except for ", /. The former
  // is below 0x30 and the latter is at 0x5C. As an optimization, for most
  // simple strings (letters, numbers, some punctuation), check for this first
  // before doing more complicated checks (more expensive checks).
  if (json_likely(uint8_t(c) >= 0x30)) {
    if (json_likely(c != '\\')) {
      *(out++) = c;
    } else {
      *(out++) = '\\';
      *(out++) = c;
    }
    return;
  }

  // In the next step, consider the characters between 0x20 and 0x30, which are
  // different punctuation and special characters. We will write most of them as
  // is, except for ", which is trivially escaped. Note that JSON allows for /
  // to be escaped as well, but most JSON serializers do not.
  if (json_likely(uint8_t(c) >= 0x20)) {
    if (json_likely(c != '"')) {
      *(out++) = c;
    } else {
      *(out++) = '\\';
      *(out++) = c;
    }
    return;
  }

  // Finally handle all control characters (below 0x20). These all need escaping
  // to some degree. There are some "popular" control characters, such as tabs,>
  // newline, and carriage return, with simple escape codes. All other control
  // characters get an escape code on the form \u00xx. Six bytes. Isch.
  const auto control_character = POPULAR_CONTROL_CHARACTERS[int(c)];
  if (json_likely(control_character != 'u')) {
    out[0] = '\\';
    out[1] = control_character;
    out += 2;
  } else {
    memcpy(out, "\\u00", 4);
    out[4] = HEX[(c >> 4)];
    out[5] = HEX[(c & 0x0F)];
    out += 6;
  }
}

json_force_inline void write_escaped_1(char *&out, const char *&begin) {
  struct blob_1_t { char a; };
  const auto b = *reinterpret_cast<const blob_1_t *>(begin);
  write_escaped_c(out, b.a);
  begin += sizeof(blob_1_t);
}

json_force_inline void write_escaped_2(char *&out, const char *&begin) {
  struct blob_2_t { char a, b; };
  const auto b = *reinterpret_cast<const blob_2_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  begin += sizeof(blob_2_t);
}

json_force_inline void write_escaped_4(char *&out, const char *&begin) {
  struct blob_4_t { char a, b, c, d; };
  const auto b = *reinterpret_cast<const blob_4_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  write_escaped_c(out, b.c);
  write_escaped_c(out, b.d);
  begin += sizeof(blob_4_t);
}

json_force_inline void write_escaped_8(char *&out, const char *&begin) {
  struct blob_8_t { char a, b, c, d, e, f, g, h; };
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

}  // namespace detail
}  // namespace json
}  // namespace spotify
