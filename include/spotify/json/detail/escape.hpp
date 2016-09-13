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
inline void escape_traits<std::string>::put(std::string &out, const char c) {
  out.push_back(c);
}

template <>
inline void escape_traits<uint8_t *>::put(uint8_t *&out, const char c) {
  *(out++) = c;
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
inline OutputType &write_escaped(OutputType &out, const uint8_t *begin, const uint8_t *end) {
  using traits = escape_traits<OutputType>;

  static const char *HEX = "0123456789ABCDEF";
  static const char POPULAR_CONTROL_CHARACTERS[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    'b', 't', 'n', 0, 'f', 'r', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  for (auto it = begin; json_likely(end != it); ++it) {
    const auto c = (*it);

    if (json_unlikely(c == '\\' || c == '"' || c == '/')) {
      traits::put(out, '\\');
      traits::put(out, c);
      continue;
    }

    if (json_likely(c >= 0x20)) {
      traits::put(out, c);
      continue;
    }

    const auto control_character = POPULAR_CONTROL_CHARACTERS[c];
    if (json_likely(control_character)) {
      traits::put(out, '\\');
      traits::put(out, control_character);
      continue;
    }

    traits::put(out, '\\');
    traits::put(out, 'u');
    traits::put(out, '0');
    traits::put(out, '0');
    traits::put(out, HEX[(c >> 4)]);
    traits::put(out, HEX[(c & 0x0F)]);
  }

  return out;
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
