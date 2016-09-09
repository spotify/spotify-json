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

#pragma once

#include <algorithm>

#include <spotify/json/decode_exception.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/escape.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace codec {

#define json_unaligned_x(ignore) true
#define JSON_STRING_SKIP_N(n, n_plus_one, type, control, goto_label) \
  control ((end - position) >= n && json_unaligned_ ## n_plus_one(position)) { \
    const auto cc = *reinterpret_cast<const type *>(position); \
    const auto has_rquote = json_haschar_ ## n(cc, '"'); \
    const auto has_escape = json_haschar_ ## n(cc, '\\'); \
    const auto is_complex = (has_rquote || has_escape); \
    if (is_complex) { goto goto_label; } \
    position += n; \
  }

class string_t final {
 public:
  using object_type = std::string;

  json_never_inline object_type decode(decoding_context &context) const {
    detail::advance_past(context, '"');
    return decode_string(context);
  }

  json_never_inline void encode(encoding_context &context, const object_type value) const {
    context.append('"');

    // Write the strings in 1024 byte chunks, so that we do not have to reserve
    // a potentially very large buffer for the escaped string. It is possible
    // that the chunking will happen in the middle of a UTF-8 multi-byte
    // character, but that is ok since write_escaped will not escape characters
    // with the high bit set, so the combined escaped string will contain the
    // correct UTF-8 characters in the end.
    auto chunk_begin = reinterpret_cast<const char *>(value.data());
    const auto string_end = chunk_begin + value.size();

    while (chunk_begin != string_end) {
      const auto chunk_end = std::min(chunk_begin + 1024, string_end);
      const auto ptr = context.reserve(6 * 1024);  // 6 is the length of \u00xx
      auto pos = ptr;  // pos will point to the end of the written escaped chunk
      detail::write_escaped(pos, chunk_begin, chunk_end);
      context.advance(pos - ptr);
      chunk_begin = chunk_end;
    }

    context.append('"');
  }

 private:
  /**
   * Skip past the bytes of the string until either a " or a \ character is
   * found. This method attempts to skip as large chunks of memory as possible
   * at each step, by making sure that the context position is aligned to the
   * appropriate address and then reading and comparing several bytes in a
   * single read operation. Upon returning, the context position will have been
   * updated to point "nearby" the stopping characters (" or \). The caller can
   * then switch to a slower algorithm to figure out what to do next.
   */
  static void skip_past_simple_characters(decoding_context &context) {
    const auto end = context.end;
    auto position = context.position;
    JSON_STRING_SKIP_N(1, 2, uint8_t,  if, done_x)
    JSON_STRING_SKIP_N(2, 4, uint16_t, if, done_x)
    JSON_STRING_SKIP_N(4, 8, uint32_t, if, done_x)
    JSON_STRING_SKIP_N(8, x, uint64_t, while, done_8)
    done_8: JSON_STRING_SKIP_N(4, x, uint32_t, if, done_x)
    done_x: context.position = position;
  }

  json_force_inline static object_type decode_string(decoding_context &context) {
    const auto begin_simple = context.position;
    skip_past_simple_characters(context);

    while (json_likely(context.remaining())) {
      switch (detail::next_unchecked(context)) {
        case '"': return std::string(begin_simple, context.position - 1);
        case '\\': return decode_escaped_string(context, begin_simple);
      }
    }

    detail::fail(context, "Unterminated string");
  }

  json_never_inline static object_type decode_escaped_string(decoding_context &context, const char *begin) {
    std::string unescaped(begin, context.position - 1);
    decode_escape(context, unescaped);

    while (json_likely(context.remaining())) {
    decode_simple:
      const auto begin_simple = context.position;
      skip_past_simple_characters(context);
      unescaped.append(begin_simple, context.position);

      while (json_likely(context.remaining())) {
        const auto character = detail::next_unchecked(context);
        switch (character) {
          case '"': return unescaped;
          case '\\': decode_escape(context, unescaped); goto decode_simple;
          default: unescaped.push_back(character); break;
        }
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
    if (json_likely(p <= 0x7F)) {
      encode_utf8_1(out, p);
    } else if (json_likely(p <= 0x07FF)) {
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

template <>
struct default_codec_t<std::string> {
  static codec::string_t codec() {
    return codec::string_t();
  }
};

}  // namespace json
}  // namespace spotify
