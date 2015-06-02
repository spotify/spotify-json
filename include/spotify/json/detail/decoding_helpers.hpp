/*
 * Copyright (c) 2014-2015 Spotify AB
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
#include <unordered_set>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/char_traits.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

/**
 * Peek at the current character that a decoding_context refers to. If the
 * decoding_context has ended, '\0' is returned. This is a useful helper when
 * while decoding it is necessary to ensure that the current character is a
 * specific one, for example '['.
 */
json_force_inline char peek(const decoding_context &context) {
  return context.position == context.end ? '\0' : *context.position;
}

/**
 * If position points to whitespace, move it forward until it reaches end or
 * non-whitespace.
 *
 * context.has_failed() must be false when this function is called.
 */
inline void advance_past_whitespace(decoding_context &context) {
  while (
      context.position != context.end &&
      char_traits<char>::is_space(*context.position)) {
    ++context.position;
  }
}

/**
 * Advance past a specific character. If the context doesn't point to a
 * matching character, advance_past will fail and mark the context as failed.
 *
 * Returns true if the operation was successful.
 *
 * context.has_failed() must be false when this function is called.
 */
inline bool advance_past(decoding_context &context, char character) {
  if (context.position == context.end) {
    context.error = std::string("Unexpected end of input, expected '") + character + "'";
    return false;
  } else if (*context.position == character) {
    context.position++;
    return true;
  } else {
    context.error = std::string("Unexpected input, expected '") + character + "'";
    return false;
  }
}

/**
 * Advance past 4 specific characters. If the context doesn't point to
 * matching characters, advance_past will fail and mark the context as failed.
 *
 * characters must be a C string of at least length 4. Only the first four
 * characters will be read.
 *
 * Returns true if the operation was successful.
 *
 * context.has_failed() must be false when this function is called.
 */
inline bool advance_past_four(decoding_context &context, const char *characters) {
  static const size_t length = 4;
  if (context.end - context.position < length) {
    context.error = std::string("Unexpected end of input, expected '") +
        std::string(characters, length) + "'";
    return false;
  } else if (*reinterpret_cast<const uint32_t *>(context.position) ==
             *reinterpret_cast<const uint32_t *>(characters)) {
    context.position += length;
    return true;
  } else {
    context.error = std::string("Unexpected input, expected '") +
        std::string(characters, length) + "'";
    return false;
  }
}

/**
 * Helper function for parsing the comma separated entities in JSON: objects
 * and arrays. intro and outro are the characters before and after the entity:
 * {} and [], respectively. parse is a callback that is called for each
 * element in the comma separated list. It should advance the parse context to
 * after that element or mark the context as failed.
 *
 * The parse callback must mark the context as failed if it sees a premature end
 * of input, otherwise this function might enter an infinite loop!
 *
 * context.has_failed() must be false when this function is called.
 */
template<typename Parse>
void advance_past_comma_separated(decoding_context &context, char intro, char outro, Parse parse) {
  if (!advance_past(context, intro)) {
    return;
  }
  advance_past_whitespace(context);
  bool has_passed_first = false;
  while (peek(context) != outro) {
    if (has_passed_first && !advance_past(context, ',')) {
      return;
    }
    has_passed_first = true;
    advance_past_whitespace(context);
    parse();
    if (context.has_failed()) {
      return;
    }
    advance_past_whitespace(context);
  }
  advance_past(context, outro);
}

/**
 * Helper for parsing JSON objects. callback is called once for each key/value
 * pair. It given the already parsed key and is expected to parse the value and
 * store it away as needed.
 *
 * After advance_past_object, context.has_failed() might be true. The callback
 * may be invoked a few times even if parsing fails later on.
 *
 * The callback may be called when context.has_failed(). In such instances, the
 * value might be an incomplete (as in not fully parsed) object.
 *
 * context.has_failed() must be false when this function is called.
 */
template<typename DecodeKey, typename Callback>
void advance_past_object(
    decoding_context &context,
    const DecodeKey &decode_key,
    const Callback &callback) {
  advance_past_comma_separated(context, '{', '}', [&]{
    auto key = decode_key(context);
    if (context.has_failed()) {
      return;
    }
    advance_past_whitespace(context);
    advance_past(context, ':');
    if (context.has_failed()) {
      return;
    }
    advance_past_whitespace(context);
    callback(std::move(key));
  });
}

inline void advance_past_true(decoding_context &context) {
  advance_past_four(context, "true");
}

inline void advance_past_false(decoding_context &context) {
  ++context.position;
  detail::advance_past_four(context, "alse");
  if (context.has_failed()) {
    --context.position;
  }
}

inline void advance_past_null(decoding_context &context) {
  advance_past_four(context, "null");
}

inline void advance_past_string_escape(decoding_context &context) {
  static const char * const kEscapeError = "Encountered unexpected character within string escape";

  advance_past(context, '\\');
  if (context.has_failed()) {
    return;
  }

  if (context.position == context.end) {
    context.error = "Unexpected end of input";
    return;
  }

  switch (*context.position) {
   case '"':
   case '\\':
   case '/':
   case 'b':
   case 'f':
   case 'n':
   case 'r':
   case 't':
    ++context.position;
    break;
   case 'u':
    ++context.position;
    for (int i = 0; i < 4; i++, ++context.position) {
      if (context.position == context.end) {
        context.error = "Unexpected end of input";
        return;
      } else if (!char_traits<char>::is_hex_digit(*context.position)) {
        context.error = kEscapeError;
        return;
      }
    }
    break;
   default:
    context.error = kEscapeError;
  }
}

inline void advance_past_string(decoding_context &context) {
  advance_past(context, '"');
  if (context.has_failed()) {
    return;
  }
  for (;;) {
    const char next = peek(context);
    if (next < 0x20) {
      context.error = "Encountered invalid string character";
      return;
    } else if (next == '"') {
      ++context.position;
      break;
    } else if (next == '\\') {
      advance_past_string_escape(context);
      if (context.has_failed()) {
        return;
      }
    } else {
      ++context.position;
    }
  }
}

inline void advance_past_number(decoding_context &context) {
  if (context.position == context.end) {
    context.error = "Unexpected end of input";
    return;
  }

  // Parse negative sign
  if (peek(context) == '-') {
    ++context.position;
  }

  // Parse integer part
  if (peek(context) == '0') {
    ++context.position;
  } else if (char_traits<char>::is_digit(peek(context))) {
    do {
      ++context.position;
    } while (char_traits<char>::is_digit(peek(context)));
  } else {
    context.error = "Expected digit";
    return;
  }

  // Parse fractional part
  if (peek(context) == '.') {
    ++context.position;
    if (!char_traits<char>::is_digit(peek(context))) {
      context.error = "Expected digit after decimal point";
      return;
    }
    do {
      ++context.position;
    } while (char_traits<char>::is_digit(peek(context)));
  }

  // Parse exp part
  const char maybe_e = peek(context);
  if (maybe_e == 'e' || maybe_e == 'E') {
    ++context.position;
    const char maybe_plus_minus = peek(context);
    if (maybe_plus_minus == '+' || maybe_plus_minus == '-') {
      ++context.position;
    }

    const char first_digit = peek(context);
    if (!char_traits<char>::is_digit(first_digit)) {
      context.error = "Expected digit after exponent sign";
    }
    do {
      ++context.position;
    } while (char_traits<char>::is_digit(peek(context)));
  }
}

/**
 * Advance past one JSON value. If parsing fails, context will be set to that
 * it has failed. If parsing suceeds, context.position will point to the
 * character after the last character of the JSON object that was parsed.
 *
 * context.has_failed() must be false when this function is called.
 */
inline void advance_past_value(decoding_context &context) {
  if (context.position == context.end) {
    context.error = "Unexpected end of input";
    return;
  }

  const char chr = *context.position;
  if (chr == '[') {
    advance_past_comma_separated(context, '[', ']', [&]{
      advance_past_value(context);
    });
  } else if (chr == '{') {
    advance_past_object(context, [](decoding_context &context) {
          advance_past_string(context);
          return 0;
        },
        [&](int &&) {
          advance_past_value(context);
        });
  } else if (chr == 't') {
    advance_past_true(context);
  } else if (chr == 'f') {
    advance_past_false(context);
  } else if (chr == 'n') {
    advance_past_null(context);
  } else if (chr == '"') {
    advance_past_string(context);
  } else if (chr == '-' || char_traits<char>::char_traits<char>::is_digit(chr)) {
    advance_past_number(context);
  } else {
    context.error = std::string("Encountered unexpected character '") + chr + "'";
  }
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
