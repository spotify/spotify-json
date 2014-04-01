// Copyright (c) 2014 Felix Bruns.

#pragma once
#ifndef JSON_ESCAPE_HPP_
#define JSON_ESCAPE_HPP_

#include <iomanip>
#include <ostream>
#include <string>

namespace json {

/**
 * \brief Simple end iterator for null-terminated strings.
 */
class null_terminated_end_iterator {
 public:
  bool operator !=(const char *it) const {
    return *it != '\0';
  }
};

/**
  * \brief Escape a string for use in a JSON string as per RFC 4627.
  *
  * This escapes control characters (0x00 through 0x1F), as well as
  * backslashes and quotation marks.
  *
  * See: http://www.ietf.org/rfc/rfc4627.txt (Section 2.5)
  */
template<typename stream_type, typename iterator, typename end_iterator>
inline void write_escaped(stream_type &stream, const iterator &begin, const end_iterator &end) {
  static const char HEX_TABLE[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };

  for (iterator it = begin; end != it; ++it) {
    const char ch = *it;

    // Escape control characters (0x00 through 0x1F)
    //
    // Note: A cast to unsigned is needed in order to format
    //       the character as a hexadecimal string instead of
    //       just writing it to the stream as is.
    if (static_cast<unsigned char>(ch) < 0x20) {
      stream.write("\\u00", 4);
      stream.put(HEX_TABLE[static_cast<unsigned char>(ch >> 4)]);
      stream.put(HEX_TABLE[static_cast<unsigned char>(ch & 0x0F)]);
    // Escape backslashes and quotation marks.
    } else if (ch == '\\' || ch == '"') {
      stream.put('\\');
      stream.put(ch);
    // Any other character does not need to be escaped.
    } else {
      stream.put(ch);
    }
  }
}

}  // namespace json

#endif  // JSON_ESCAPE_HPP_
