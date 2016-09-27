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

#include <spotify/json/detail/skip_value.hpp>

#include <limits>

#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/detail/stack.hpp>

namespace spotify {
namespace json {
namespace detail {
namespace {

void skip_unicode_escape(decode_context &context) {
  require_bytes<4>(context, "\\u must be followed by 4 hex digits");
  const bool h0 = char_traits<char>::is_hex_digit(*(context.position++));
  const bool h1 = char_traits<char>::is_hex_digit(*(context.position++));
  const bool h2 = char_traits<char>::is_hex_digit(*(context.position++));
  const bool h3 = char_traits<char>::is_hex_digit(*(context.position++));
  fail_if(context, !(h0 && h1 && h2 && h3), "\\u must be followed by 4 hex digits");
}

void skip_escape(decode_context &context) {
  switch (next(context, "Unterminated string")) {
    case '"':  break;
    case '/':  break;
    case 'b':  break;
    case 'f':  break;
    case 'n':  break;
    case 'r':  break;
    case 't':  break;
    case '\\': break;
    case 'u': skip_unicode_escape(context); break;
    default: detail::fail(context, "Invalid escape character", -1);
  }
}

void skip_string(decode_context &context) {
  skip_1(context, '"');

  while (json_likely(context.remaining())) {
    detail::skip_any_simple_characters(context);
    switch (next(context, "Unterminated string")) {
      case '"': return;
      case '\\': skip_escape(context); break;
      default: json_unreachable();
    }
  }

  detail::fail(context, "Unterminated string");
}

void skip_number(decode_context &context) {
  using traits = char_traits<char>;

  // Parse negative sign
  if (peek(context) == '-') {
    ++context.position;
  }

  // Parse integer part
  if (peek(context) == '0') {
    ++context.position;
  } else {
    fail_if(context, !traits::is_digit(peek(context)), "Expected digit");
    do { ++context.position; } while (traits::is_digit(peek(context)));
  }

  // Parse fractional part
  if (peek(context) == '.') {
    ++context.position;
    fail_if(context, !traits::is_digit(peek(context)), "Expected digit after decimal point");
    do { ++context.position; } while (traits::is_digit(peek(context)));
  }

  // Parse exp part
  const char maybe_e = peek(context);
  if (maybe_e == 'e' || maybe_e == 'E') {
    ++context.position;
    const char maybe_plus_minus = peek(context);
    if (maybe_plus_minus == '+' || maybe_plus_minus == '-') {
      ++context.position;
    }

    fail_if(context, !traits::is_digit(peek(context)), "Expected digit after exponent sign");
    do { ++context.position; } while (char_traits<char>::is_digit(peek(context)));
  }
}

/**
 * Advance past one simple JSON value, that is any value that is not an object
 * {} or an array []. If parsing fails, context will be set to that it has
 * failed. If parsing suceeds, context.position will point to the character
 * after the last character of the JSON object that was parsed.
 *
 * context.has_failed() must be false when this function is called.
 */
void skip_simple_value(decode_context &context) {
  switch (peek(context)) {
    case '-':  // fallthrough
    case '0': case '1': case '2': case '3': case '4':  // fallthrough
    case '5': case '6': case '7': case '8': case '9': skip_number(context); break;
    case '"': skip_string(context); break;
    case 'f': skip_false(context); break;
    case 't': skip_true(context); break;
    case 'n': skip_null(context); break;
    default: fail(context, std::string("Encountered token '") + peek(context) + "'");
  }
}

}  // namespace

void skip_value(decode_context &context) {
  enum state {
    done = 0,
    want = 1 << 0,
    need = 1 << 1,
    read_sep = 1 << 2,
    read_key = 1 << 3,
    read_val = 1 << 4,

    want_sep = want | read_sep,
    want_key = want | read_key,
    need_key = need | read_key,
    want_val = want | read_val,
    need_val = need | read_val
  };

  // We can deal with the first 64 nesting levels {[[{[[ ... ]]}]]} without heap
  // allocations. Most reasonable JSON will have way less than this, but in case
  // we encounter an unusual JSON file (perhaps one designed to stack overflow),
  // the nesting stack will be moved over to the heap.
  detail::stack<char, 64> stack;

  auto inside = 0;
  auto closer = int_fast16_t(std::numeric_limits<int16_t>::max());  // a value outside the range of a 'char'
  auto pstate = need_val;

  while (json_likely(context.remaining() && pstate != done)) {
    if (json_likely(inside)) {
      skip_any_whitespace(context);
    }

    const auto c = peek_unchecked(context);

    if (c == ',' && (pstate & read_sep)) {
      skip_unchecked_1(context);
      pstate = (inside == '{' ? need_key : need_val);
      continue;
    }

    if (c == '"' && (pstate & read_key)) {
      skip_string(context);
      skip_any_whitespace(context);
      skip_1(context, ':');
      pstate = need_val;
      continue;
    }

    if (c == closer && !(pstate & need)) {
      skip_unchecked_1(context);
      inside = stack.pop();
      closer = inside + 2;  // '{' + 2 == '}', '[' + 2 == ']'
      pstate = (inside ? want_sep : done);
      continue;
    }

    fail_if(context, pstate & read_key, "Expected '\"'");
    fail_if(context, pstate & read_sep, inside == '{' ?
        "Expected ',' or '}'" :
        "Expected ',' or ']");

    if (c == '{' || c == '[') {
      skip_unchecked_1(context);
      stack.push(inside);
      inside = c;
      closer = inside + 2;  // '{' + 2 == '}', '[' + 2 == ']'
      pstate = (inside == '{' ? want_key : want_val);
      continue;
    }

    skip_simple_value(context);
    pstate = (inside ? want_sep : done);
  }

  fail_if(context, inside == '{', "Expected '}'");
  fail_if(context, inside == '[', "Expected ']'");
  fail_if(context, pstate != done, "Unexpected EOF");
}

}  // detail
}  // json
}  // spotify
