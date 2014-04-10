/*
 * Copyright (c) 2014 Spotify AB
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

#include <string>

namespace spotify {
namespace json {
namespace detail {

/**
 * \brief Simple end iterator for null-terminated strings.
 */
class null_terminated_end_iterator {
 public:
  bool operator !=(const char *it) const {
    return *it != '\0';
  }
};

template<typename OutputType>
struct escape_traits {
  static void write(OutputType &out, const char *data, size_t length) {
    out.write(data, length);
  }
  static void put(OutputType &out, char ch) {
    out.put(ch);
  }
};

template<>
inline void escape_traits<std::string>::write(
    std::string &out, const char *data, size_t length) {
  out.append(data, length);
}

template<>
inline void escape_traits<std::string>::put(
    std::string &out, char ch) {
  out.push_back(ch);
}

/**
  * \brief Escape a string for use in a JSON string as per RFC 4627.
  *
  * This escapes control characters (0x00 through 0x1F), as well as
  * backslashes and quotation marks.
  *
  * See: http://www.ietf.org/rfc/rfc4627.txt (Section 2.5)
  */
template<typename OutputType, typename InputIterator, typename InputEndIterator>
inline OutputType &write_escaped(OutputType &out, const InputIterator &begin, const InputEndIterator &end) {
  typedef escape_traits<OutputType> traits;

  static const char *HEX = "0123456789ABCDEF";

  for (InputIterator it = begin; end != it; ++it) {
    const unsigned char ch = static_cast<unsigned char>(*it);
    const bool is_control_character(ch < 0x20);
    const bool is_special_character(ch == '\\' || ch == '"');

    if (is_control_character) {
      traits::write(out, "\\u00", 4);
      traits::put(out, HEX[(ch >> 4)]);
      traits::put(out, HEX[(ch & 0x0F)]);
    } else if (is_special_character) {
      traits::put(out, '\\');
      traits::put(out, ch);
    } else {
      traits::put(out, ch);
    }
  }

  return out;
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
