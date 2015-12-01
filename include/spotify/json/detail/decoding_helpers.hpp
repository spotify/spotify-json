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

#include <cstddef>
#include <string>
#include <unordered_set>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/char_traits.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename string_type>
json_never_inline json_noreturn void fail(
    const decoding_context &context,
    const string_type &error,
    const ptrdiff_t d = 0) {
  throw decode_exception(error, context.offset(d));
}

template <typename string_type>
json_force_inline void fail_if(
    const decoding_context &context,
    const bool condition,
    const string_type &error,
    const ptrdiff_t d = 0) {
  if (json_unlikely(condition)) {
    fail(context, error, d);
  }
}

template <size_t num_required_bytes, typename string_type>
json_force_inline void require_bytes(
    const decoding_context &context,
    const string_type &error = "Unexpected end of input") {
  fail_if(context, context.remaining() < num_required_bytes, error);
}

template <size_t num_required_bytes>
json_force_inline void require_bytes(const decoding_context &context) {
  require_bytes<num_required_bytes>(context, "Unexpected end of input");
}

json_force_inline char peek_unchecked(const decoding_context &context) {
  return *context.position;
}

/**
 * Peek at the current character that a decoding_context refers to. If the
 * decoding_context has ended, '\0' is returned. This is a useful helper when
 * while decoding it is necessary to ensure that the current character is a
 * specific one, for example '['.
 */
json_force_inline char peek(const decoding_context &context) {
  return (context.remaining() ? peek_unchecked(context) : 0);
}

json_force_inline char next_unchecked(decoding_context &context) {
  return *(context.position++);
}

template <typename string_type>
json_force_inline char next(decoding_context &context, const string_type &error) {
  require_bytes<1>(context, error);
  return next_unchecked(context);
}

json_force_inline char next(decoding_context &context) {
  return next(context, "Unexpected end of input");
}

json_force_inline void skip_unchecked(decoding_context &context, const size_t num_bytes) {
  context.position += num_bytes;
}

json_force_inline void skip_unchecked(decoding_context &context) {
  context.position++;
}

json_force_inline void skip(decoding_context &context, const size_t num_bytes) {
  fail_if(context, context.remaining() < num_bytes, "Unexpected end of input");
  skip_unchecked(context, num_bytes);
}

json_force_inline void skip(decoding_context &context) {
  require_bytes<1>(context, "Unexpected end of input");
  context.position++;
}

/**
 * If position points to whitespace, move it forward until it reaches end or
 * non-whitespace.
 */
inline void advance_past_whitespace(decoding_context &context) {
  while (char_traits<char>::is_space(peek(context))) {
    skip(context);
  }
}

/**
 * Advance past a specific character. If the context position does not point to
 * a matching character, a decode_exception is thrown.
 */
inline void advance_past(decoding_context &context, char character) {
  fail_if(context, next(context) != character, "Unexpected input", -1);
}

/**
 * Advance past 4 specific characters. If the context position does not point to
 * matching characters, a decode_exception is thrown. 'characters' must be a C
 * string of at least length 4. Only the first four characters will be read.
 */
inline void advance_past_four(decoding_context &context, const char *characters) {
  require_bytes<4>(context);
  const char c0 = *(context.position++);
  const char c1 = *(context.position++);
  const char c2 = *(context.position++);
  const char c3 = *(context.position++);
  fail_if(
      context,
      c0 != characters[0] ||
      c1 != characters[1] ||
      c2 != characters[2] ||
      c3 != characters[3],
      "Unexpected input",
      -4);
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
  advance_past(context, intro);
  advance_past_whitespace(context);

  if (peek(context) != outro) {
    parse();
    advance_past_whitespace(context);

    while (peek(context) != outro) {
      advance_past(context, ',');
      advance_past_whitespace(context);
      parse();
      advance_past_whitespace(context);
    }
  }

  context.position++;
}

/**
 * Helper for parsing JSON objects. callback is called once for each key/value
 * pair. It is given the already parsed key and is expected to parse the value
 * and store it away as needed. The callback may be invoked a few times even if
 * parsing fails later on.
 */
template<typename DecodeKey, typename Callback>
void advance_past_object(
    decoding_context &context,
    const DecodeKey &decode_key,
    const Callback &callback) {
  advance_past_comma_separated(context, '{', '}', [&]{
    auto key = decode_key(context);
    advance_past_whitespace(context);
    advance_past(context, ':');
    advance_past_whitespace(context);
    callback(std::move(key));
  });
}

inline void advance_past_true(decoding_context &context) {
  advance_past_four(context, "true");
}

inline void advance_past_false(decoding_context &context) {
  context.position++;  // skip past the 'f' in 'false', we know it is there
  advance_past_four(context, "alse");
}

inline void advance_past_null(decoding_context &context) {
  advance_past_four(context, "null");
}

inline void advance_past_string_escape_after_slash(decoding_context &context) {
  switch (next(context, "Unterminated string")) {
    case '"':
    case '\\':
    case '/':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
      break;
   case 'u': {
      require_bytes<4>(context, "\\u must be followed by 4 hex digits");
      const bool h0 = char_traits<char>::is_hex_digit(*(context.position++));
      const bool h1 = char_traits<char>::is_hex_digit(*(context.position++));
      const bool h2 = char_traits<char>::is_hex_digit(*(context.position++));
      const bool h3 = char_traits<char>::is_hex_digit(*(context.position++));
      fail_if(context, !(h0 && h1 && h2 && h3), "\\u must be followed by 4 hex digits");
      break;
    }
   default:
    fail(context, "Invalid escape character", -1);
  }
}

inline void advance_past_string_escape(decoding_context &context) {
  advance_past(context, '\\');
  advance_past_string_escape_after_slash(context);
}

inline void advance_past_string(decoding_context &context) {
  advance_past(context, '"');
  for (;;) {
    const char c = next(context, "Unterminated string");
    fail_if(context, static_cast<unsigned char>(c) < 0x20,
        "Encountered invalid string character");
    switch (c) {
      case '"': return;
      case '\\': advance_past_string_escape_after_slash(context); break;
    }
  }
}

inline void advance_past_number(decoding_context &context) {
  require_bytes<1>(context);

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
    fail(context, "Expected digit");
  }

  // Parse fractional part
  if (peek(context) == '.') {
    ++context.position;
    if (!char_traits<char>::is_digit(peek(context))) {
      fail(context, "Expected digit after decimal point");
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
      fail(context, "Expected digit after exponent sign");
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
  require_bytes<1>(context);
  const char c = *context.position;
  if (c == '[') {
    advance_past_comma_separated(context, '[', ']', [&]{
      advance_past_value(context);
    });
  } else if (c == '{') {
    advance_past_object(context, [](decoding_context &context) {
          advance_past_string(context);
          return 0;
        },
        [&](int &&) {
          advance_past_value(context);
        });
  } else if (c == 't') {
    advance_past_true(context);
  } else if (c == 'f') {
    advance_past_false(context);
  } else if (c == 'n') {
    advance_past_null(context);
  } else if (c == '"') {
    advance_past_string(context);
  } else if (c == '-' || char_traits<char>::is_digit(c)) {
    advance_past_number(context);
  } else {
    fail(context, std::string("Encountered unexpected character '") + c + "'");
  }
}

template<typename T>
struct has_should_encode_method {
  template<typename U>
  static auto test(int) -> decltype(
      std::declval<U>().should_encode(std::declval<typename U::object_type>()),
      std::true_type());

  template<typename>
  static std::false_type test(...);

 public:
  static constexpr bool value = std::is_same<decltype(test<T>(0)),std::true_type>::value;
};

template<typename Codec>
typename std::enable_if<!has_should_encode_method<Codec>::value, bool>::type
should_encode(const Codec &codec, const typename Codec::object_type &value) {
  return true;
}

template<typename Codec>
typename std::enable_if<has_should_encode_method<Codec>::value, bool>::type
should_encode(const Codec &codec, const typename Codec::object_type &value) {
  return codec.should_encode(value);
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
