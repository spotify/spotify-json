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

#include <spotify/json/decode_exception.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>

namespace spotify {
namespace json {
namespace codec {

class string_t final : public detail::primitive_encoder<std::string> {
 public:
  object_type decode(decoding_context &context) const {
    detail::advance_past(context, '"');
    return decode_string(context);
  }

 private:
  static object_type decode_string(decoding_context &context) {
    const char *begin = context.position;

    while (context.position != context.end) {
      switch (*context.position) {
        case '"': return std::string(begin, context.position++);
        case '\\': return decode_escaped_string(context, begin);
        default: context.position++; break;
      }
    }

    detail::fail(context, "Unterminated string");
  }

  static object_type decode_escaped_string(decoding_context &context, const char *begin) {
    std::string unescaped(begin, context.position);

    while (context.position != context.end) {
      const auto character = *(context.position++);
      switch (character) {
        case '"': return unescaped;
        case '\\': decode_escape(context, unescaped); break;
        default: unescaped.push_back(character); break;
      }
    }

    detail::fail(context, "Unterminated string");
  }

  static void decode_escape(decoding_context &context, std::string &out) {
    const auto escape_character = detail::next(context, "Unterminated string");
    switch (escape_character) {
      case '"':  out.push_back('"');  break;
      case '/':  out.push_back('/');  break;
      case 'b':  out.push_back('\b'); break;
      case 'f':  out.push_back('\f'); break;
      case 'n':  out.push_back('\n'); break;
      case 'r':  out.push_back('\r'); break;
      case 't':  out.push_back('\t'); break;
      case '\\': out.push_back('\\'); break;
      case 'u': decode_unicode_escape(context, out); break;
      default: detail::fail(context, "Invalid escape character", -1);
    }
  }

  static uint8_t decode_hex_nibble(decoding_context &context, const char c) {
    if (c >= '0' && c <= '9') { return c - '0'; }
    if (c >= 'a' && c <= 'f') { return c - 'a' + 0xA; }
    if (c >= 'A' && c <= 'F') { return c - 'A' + 0xA; }
    detail::fail(context, "\\u must be followed by 4 hex digits");
  }

  static void decode_unicode_escape(decoding_context &context, std::string &out) {
    detail::require_bytes<4>(context, "\\u must be followed by 4 hex digits");
    const auto a = decode_hex_nibble(context, *(context.position++));
    const auto b = decode_hex_nibble(context, *(context.position++));
    const auto c = decode_hex_nibble(context, *(context.position++));
    const auto d = decode_hex_nibble(context, *(context.position++));
    const auto p = unsigned((a << 12) | (b << 8) | (c << 4) | d);
    encode_utf8(context, out, p);
  }

  static void encode_utf8(decoding_context &context, std::string &out, unsigned p) {
    if (p <= 0x7F) {
      encode_utf8_1(out, p);
    } else if (p <= 0x07FF) {
      encode_utf8_2(out, p);
    } else {
      encode_utf8_3(out, p);
    }
  }

  static void encode_utf8_1(std::string &out, unsigned p) {
    const char c0 = (p & 0x7F);
    out.push_back(c0);
  }

  static void encode_utf8_2(std::string &out, unsigned p) {
    const char c0 = 0xC0 | ((p >> 6) & 0x1F);
    const char c1 = 0x80 | ((p >> 0) & 0x3F);
    const char cc[] = { c0, c1 };
    out.append(&cc[0], 2);
  }

  static void encode_utf8_3(std::string &out, unsigned p) {
    const char c0 = 0xE0 | ((p >> 12) & 0x0F);
    const char c1 = 0x80 | ((p >>  6) & 0x3F);
    const char c2 = 0x80 | ((p >>  0) & 0x3F);
    const char cc[] = { c0, c1, c2 };
    out.append(&cc[0], 3);
  }
};

inline string_t string() {
  return string_t();
}

}  // namespace codec

template<>
struct default_codec_t<std::string> {
  static codec::string_t codec() {
    return codec::string_t();
  }
};

}  // namespace json
}  // namespace spotify
