/*
 * Copyright (c) 2015-2019 Spotify AB
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

#include <spotify/json/codec/string.hpp>

#include <spotify/json/decode_exception.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/escape.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/detail/skip_chars.hpp>

namespace spotify {
namespace json {
namespace codec {
namespace {

bool is_high_surrogate(unsigned p) {
  return (p & 0xFC00) == 0xD800;
}

bool is_low_surrogate(unsigned p) {
  return (p & 0xFC00) == 0xDC00;
}

uint32_t codepoint_from_surrogate_pair(uint32_t high, uint32_t low) {
  return (((high & 0x03FF) << 10) | (low & 0x03FF)) + 0x10000;
}

uint8_t decode_hex_nibble(decode_context &context, const char c) {
  if (c >= '0' && c <= '9') { return c - '0'; }
  if (c >= 'a' && c <= 'f') { return c - 'a' + 0xA; }
  if (c >= 'A' && c <= 'F') { return c - 'A' + 0xA; }
  detail::fail(context, "\\u must be followed by 4 hex digits");
}

unsigned decode_hex_number(decode_context &context) {
  detail::require_bytes<4>(context, "\\u must be followed by 4 hex digits");
  const auto a = decode_hex_nibble(context, *(context.position++));
  const auto b = decode_hex_nibble(context, *(context.position++));
  const auto c = decode_hex_nibble(context, *(context.position++));
  const auto d = decode_hex_nibble(context, *(context.position++));
  return unsigned((a << 12) | (b << 8) | (c << 4) | d);
}

void encode_utf8_4(std::string &out, uint32_t p) {
  const char c0 = 0xF0 | ((p >> 18) & 0x07);
  const char c1 = 0x80 | ((p >> 12) & 0x3F);
  const char c2 = 0x80 | ((p >>  6) & 0x3F);
  const char c3 = 0x80 | ((p >>  0) & 0x3F);
  const char cc[] = { c0, c1, c2, c3 };
  out.append(&cc[0], 4);
}

void encode_utf8_3(std::string &out, unsigned p) {
  const char c0 = 0xE0 | ((p >> 12) & 0x0F);
  const char c1 = 0x80 | ((p >>  6) & 0x3F);
  const char c2 = 0x80 | ((p >>  0) & 0x3F);
  const char cc[] = { c0, c1, c2 };
  out.append(&cc[0], 3);
}

void encode_utf8_2(std::string &out, unsigned p) {
  const char c0 = 0xC0 | ((p >> 6) & 0x1F);
  const char c1 = 0x80 | ((p >> 0) & 0x3F);
  const char cc[] = { c0, c1 };
  out.append(&cc[0], 2);
}

void encode_utf8_1(std::string &out, unsigned p) {
  const char c0 = (p & 0x7F);
  out.push_back(c0);
}

void encode_utf8(std::string &out, unsigned p) {
  if (json_likely(p <= 0x7F)) {
    encode_utf8_1(out, p);
  } else if (json_likely(p <= 0x07FF)) {
    encode_utf8_2(out, p);
  } else {
    encode_utf8_3(out, p);
  }
}

bool handle_surrogate_pair(decode_context &context, std::string &out, unsigned p) {
  if (json_unlikely(is_high_surrogate(p))) {
    // Parse low surrogate
    if (detail::peek_2(context, '\\', 'u')) {
      detail::skip_unchecked_n(context, 2);
      const auto n = decode_hex_number(context);
      if (json_likely(is_low_surrogate(n))) {
        // Any Unicode codepoint encoded by a surrogate pair is 4 bytes in UTF-8
        encode_utf8_4(out, codepoint_from_surrogate_pair(p, n));
        return true;
      } else {
        // Rewind context to before the escape sequence
        context.position -= 6;
      }
    }
  }

  return false;
}

void decode_unicode_escape(decode_context &context, std::string &out) {
  const auto p = decode_hex_number(context);
  if (json_likely(!handle_surrogate_pair(context, out, p))) {
    encode_utf8(out, p);
  }
}

void decode_escape(decode_context &context, std::string &out) {
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

string_t::object_type decode_escaped_string(decode_context &context, const char *begin) {
  std::string unescaped(begin, context.position - 1);
  decode_escape(context, unescaped);

  while (json_likely(context.remaining())) {
    const auto begin_simple = context.position;
    detail::skip_any_simple_characters(context);
    unescaped.append(begin_simple, context.position);

    switch (detail::next(context, "Unterminated string")) {
      case '"': return unescaped;
      case '\\': decode_escape(context, unescaped); break;
      default: json_unreachable();
    }
  }

  detail::fail(context, "Unterminated string");
}

string_t::object_type decode_string(decode_context &context) {
  const auto begin_simple = context.position;
  detail::skip_any_simple_characters(context);

  switch (detail::next(context, "Unterminated string")) {
    case '"': return std::string(begin_simple, context.position - 1);
    case '\\': return decode_escaped_string(context, begin_simple);
    default: json_unreachable();
  }
}

}  // namespace

string_t::object_type string_t::decode(decode_context &context) const {
  detail::skip_1(context, '"');
  return decode_string(context);
}

void string_t::encode(encode_context &context, const object_type value) const {
  context.append('"');

  // Write the strings in 1024 byte chunks, so that we do not have to reserve a
  // potentially very large buffer for the escaped string. It is possible that
  // the chunking will happen in the middle of a UTF-8 multi-byte character, but
  // that is ok since write_escaped will not escape characters with the high bit
  // set, so the combined escaped string contains the correct UTF-8 characters
  // in the end.
  auto chunk_begin = value.data();
  const auto string_end = chunk_begin + value.size();

  while (chunk_begin != string_end) {
    const auto chunk_end = std::min(chunk_begin + 1024, string_end);
    detail::write_escaped(context, chunk_begin, chunk_end);
    chunk_begin = chunk_end;
  }

  context.append('"');
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
