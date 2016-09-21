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

#include <type_traits>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/codec/omit.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

decoding_context make_context(const char *str) {
  return decoding_context(str, str + strlen(str));
}

template <typename Advance>
void verify_advance(const Advance &advance, const char *str) {
  auto ctx = make_context(str);
  const auto original_ctx = ctx;
  advance(ctx);
  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

template <typename Advance>
void verify_advance_fail(const Advance &advance, const char *str) {
  auto ctx = make_context(str);
  BOOST_CHECK_THROW(advance(ctx), decode_exception);
}

template <typename Advance>
void verify_advance_partial(const Advance &advance, const char *str, size_t len) {
  auto ctx = make_context(str);
  const auto original_ctx = ctx;
  advance(ctx);
  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + len);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

}  // namespace

/*
 * Peek
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_peek_with_empty_input) {
  BOOST_CHECK_EQUAL(peek(make_context("")), '\0');
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_peek_at_last_character) {
  BOOST_CHECK_EQUAL(peek(make_context("a")), 'a');
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_peek) {
  BOOST_CHECK_EQUAL(peek(make_context("ab")), 'a');
}

/*
 * Next
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_next_with_empty_input_should_fail) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(next(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_next_at_last_character) {
  auto ctx = make_context("a");
  BOOST_CHECK_EQUAL(next(ctx), 'a');
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_next) {
  auto ctx = make_context("ab");
  BOOST_CHECK_EQUAL(next(ctx), 'a');
  BOOST_CHECK_EQUAL(next(ctx), 'b');
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_next_unchecked_at_last_character) {
  auto ctx = make_context("a");
  BOOST_CHECK_EQUAL(next_unchecked(ctx), 'a');
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_next_unchecked) {
  auto ctx = make_context("ab");
  BOOST_CHECK_EQUAL(next_unchecked(ctx), 'a');
  BOOST_CHECK_EQUAL(next_unchecked(ctx), 'b');
  BOOST_CHECK(!ctx.remaining());
}

/*
 * Skip
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip_with_empty_input_should_fail) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(skip(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip_at_last_character) {
  auto ctx = make_context("a");
  skip(ctx);
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip) {
  auto ctx = make_context("ab");
  skip(ctx);
  skip(ctx);
  BOOST_CHECK(!ctx.remaining());
}

/*
 * Advance past whitespace
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip_past_whitespace_with_empty_input) {
  auto ctx = make_context("");
  const auto original_ctx = ctx;
  skip_past_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip_past_whitespace_with_non_whitespace_input) {
  auto ctx = make_context("a");
  const auto original_ctx = ctx;
  skip_past_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip_past_whitespace_with_whitespace_input_to_end) {
  auto ctx = make_context(" \t\r\n");
  const auto original_ctx = ctx;
  skip_past_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_skip_past_whitespace_with_whitespace_input) {
  auto ctx = make_context(" a\t\r\n");
  const auto original_ctx = ctx;
  skip_past_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 1);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

/*
 * Advance past single character
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_with_empty_input) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(advance_past(ctx, 'a'), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_with_matching_input_to_end) {
  auto ctx = make_context("a");
  const auto original_ctx = ctx;
  advance_past(ctx, 'a');

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_with_matching_input) {
  auto ctx = make_context("aaa");
  const auto original_ctx = ctx;
  advance_past(ctx, 'a');

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 1);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_with_nonmatching_input) {
  auto ctx = make_context("b");
  BOOST_CHECK_THROW(advance_past(ctx, 'a'), decode_exception);
}

/*
 * Advance past four characters
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_word_with_empty_input) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(advance_past_four(ctx, "aaaa"), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_word_with_too_short_input) {
  auto ctx = make_context("abc");
  BOOST_CHECK_THROW(advance_past_four(ctx, "abcd"), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_word_with_matching_input_to_end) {
  auto ctx = make_context("abcd");
  const auto original_ctx = ctx;
  advance_past_four(ctx, "abcd");

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_word_with_matching_input) {
  auto ctx = make_context("abcde");
  const auto original_ctx = ctx;
  advance_past_four(ctx, "abcd");

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 4);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_word_with_nonmatching_input) {
  auto ctx = make_context("abcD");
  BOOST_CHECK_THROW(advance_past_four(ctx, "abcd"), decode_exception);
}

/*
 * Advance past comma separated
 */

namespace {

void dont_call() {
  BOOST_CHECK(!"Should not call this function");
}

/**
 * Parse a string of the format "<a,b,c,d...>" and check that the parse is
 * successful.
 */
void parse(const char *string) {
  auto ctx = make_context(string);
  const auto original_ctx = ctx;

  char expected_char =  'a';
  advance_past_comma_separated(ctx, '<', '>', [&]{
    advance_past(ctx, expected_char++);
  });

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_empty_input) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', &dont_call), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_wrong_first_character) {
  auto ctx = make_context(">");
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', &dont_call), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_immediate_end) {
  auto ctx = make_context("<>");
  const auto original_ctx = ctx;
  advance_past_comma_separated(ctx, '<', '>', &dont_call);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_before_first) {
  auto ctx = make_context(" <>");
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', &dont_call), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_after_first) {
  auto ctx = make_context("< >");
  const auto original_ctx = ctx;
  advance_past_comma_separated(ctx, '<', '>', &dont_call);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_after_last) {
  auto ctx = make_context("<> ");
  const auto original_ctx = ctx;
  advance_past_comma_separated(ctx, '<', '>', &dont_call);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 2);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_single_element) {
  parse("<a>");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_two_elements) {
  parse("<a,b>");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_before_first_element) {
  parse("< a,b>");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_before_comma) {
  parse("<a ,b>");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_after_comma) {
  parse("<a, b>");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_whitespace_before_last) {
  parse("<a,b >");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_wrong_last_character) {
  auto ctx = make_context("<<");
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', [&]{}), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_missing_last_character) {
  auto ctx = make_context("<");
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', [&]{}), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_trailing_comma) {
  auto ctx = make_context("<a,>");
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', [&]{
    advance_past(ctx, 'a');
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_comma_separated_with_failing_inner_parse) {
  auto ctx = make_context("<a,a>");
  bool was_called_already = false;
  BOOST_CHECK_THROW(advance_past_comma_separated(ctx, '<', '>', [&]{
    BOOST_CHECK(!was_called_already);
    was_called_already = true;
    advance_past(ctx, 'b');
  }), decode_exception);
}

/*
 * Advance past object
 */

namespace {

bool decode_boolean(decoding_context &context) {
  return codec::boolean().decode(context);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_empty_object) {
  auto ctx = make_context("{}");
  const auto original_ctx = ctx;
  advance_past_object<codec::omit_t<bool>>(ctx, [&](bool &&key) {
    BOOST_CHECK(!"Should not be called");
  });
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_with_single_value) {
  auto ctx = make_context("{true:false}");
  const auto original_ctx = ctx;
  bool has_been_called_already = false;
  advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    const auto value = decode_boolean(ctx);
    BOOST_CHECK(!has_been_called_already);
    has_been_called_already = true;
    BOOST_CHECK_EQUAL(key, true);
    BOOST_CHECK_EQUAL(value, false);
  });
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_with_two_values) {
  auto ctx = make_context("{true:false,false:true}");
  const auto original_ctx = ctx;
  size_t times_called = 0;
  advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    const auto value = decode_boolean(ctx);
    BOOST_CHECK_EQUAL(key, !times_called);
    BOOST_CHECK_EQUAL(value, !!times_called);
    times_called++;
  });
  BOOST_CHECK_EQUAL(times_called, 2);
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_with_whitespace) {
  auto ctx = make_context("{ true : false , false : true }");
  const auto original_ctx = ctx;
  size_t times_called = 0;
  advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    const auto value = decode_boolean(ctx);
    BOOST_CHECK_EQUAL(key, !times_called);
    BOOST_CHECK_EQUAL(value, !!times_called);
    times_called++;
  });
  BOOST_CHECK_EQUAL(times_called, 2);
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_with_broken_key) {
  auto ctx = make_context("{tru:false}");
  BOOST_CHECK_THROW(advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    BOOST_CHECK(!"Should not be called");
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_with_broken_value) {
  auto ctx = make_context("{true:fals}");
  BOOST_CHECK_THROW(advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    decode_boolean(ctx);
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_without_colon) {
  auto ctx = make_context("{truefalse}");
  BOOST_CHECK_THROW(advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    BOOST_CHECK(!"Should not be called");
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_object_without_ending_brace) {
  auto ctx = make_context("{true:false");
  BOOST_CHECK_THROW(advance_past_object<codec::boolean_t>(ctx, [&](bool &&key) {
    decode_boolean(ctx);
  }), decode_exception);
}

/*
 * advance_past_true, advance_past_false and advance_past_null are tested
 * indirectly in their respective codec tests.
 */

/*
 * Advance past string escape
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_empty) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(advance_past_string_escape(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_non_escape) {
  auto ctx = make_context("\"");
  BOOST_CHECK_THROW(advance_past_string_escape(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_just_backslash) {
  auto ctx = make_context("\\");
  BOOST_CHECK_THROW(advance_past_string_escape(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_invalid_character) {
  auto ctx = make_context("\\a");
  BOOST_CHECK_THROW(advance_past_string_escape(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_single_character_escape) {
  verify_advance(&advance_past_string_escape, "\\\"");
  verify_advance(&advance_past_string_escape, "\\\\");
  verify_advance(&advance_past_string_escape, "\\b");
  verify_advance(&advance_past_string_escape, "\\f");
  verify_advance(&advance_past_string_escape, "\\n");
  verify_advance(&advance_past_string_escape, "\\r");
  verify_advance(&advance_past_string_escape, "\\t");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_unicode_escape) {
  verify_advance(&advance_past_string_escape, "\\u0000");
  verify_advance(&advance_past_string_escape, "\\u1234");
  verify_advance(&advance_past_string_escape, "\\uffff");
  verify_advance(&advance_past_string_escape, "\\uABCD");
  verify_advance(&advance_past_string_escape, "\\u0F0f");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape_invalid_unicode_escape) {
  verify_advance_fail(&advance_past_string_escape, "\\u0");
  verify_advance_fail(&advance_past_string_escape, "\\u000");
  verify_advance_fail(&advance_past_string_escape, "\\ug000");
  verify_advance_fail(&advance_past_string_escape, "\\u000G");
}

/*
 * Advance past string
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_empty) {
  verify_advance_fail(&advance_past_string, "");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_invalid_character) {
  verify_advance_fail(&advance_past_string, "a");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_only_opening) {
  verify_advance_fail(&advance_past_string, "\"");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_no_characters) {
  verify_advance(&advance_past_string, "\"\"");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_some_characters) {
  verify_advance(&advance_past_string, "\"abc\"");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_invalid_characters) {
  verify_advance_fail(&advance_past_string, "\a");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_escape) {
  verify_advance(&advance_past_string, R"("\"")");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_invalid_escape) {
  verify_advance_fail(&advance_past_string, R"("\a")");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_string_utf8) {
  verify_advance(&advance_past_string, u8"\"\u9E21\"");
}

/*
 * Advance past number
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_empty) {
  verify_advance_fail(&advance_past_number, "");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_invalid_character) {
  verify_advance_fail(&advance_past_number, "a");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_just_negative_sign) {
  verify_advance_fail(&advance_past_number, "-");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_zero) {
  verify_advance(&advance_past_number, "0");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_negative_zero) {
  verify_advance(&advance_past_number, "-0");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_one) {
  verify_advance(&advance_past_number, "1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_zero_with_trailing_digit) {
  verify_advance_partial(&advance_past_number, "01", 1);
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_just_decimal_dot) {
  verify_advance_fail(&advance_past_number, ".");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_decimal_dot_before_int) {
  verify_advance_fail(&advance_past_number, ".1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_decimal_dot_last) {
  verify_advance_fail(&advance_past_number, "1.");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_decimal_dot) {
  verify_advance(&advance_past_number, "1.0");
  verify_advance(&advance_past_number, "1.1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_two_decimal_dots) {
  verify_advance_fail(&advance_past_number, "1..1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_just_exp_e) {
  verify_advance_fail(&advance_past_number, "e");
  verify_advance_fail(&advance_past_number, "E");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_number_then_exp_e) {
  verify_advance_fail(&advance_past_number, "1e");
  verify_advance_fail(&advance_past_number, "1E");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_exp_e_then_number) {
  verify_advance_fail(&advance_past_number, "e1");
  verify_advance_fail(&advance_past_number, "E1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_exp) {
  verify_advance(&advance_past_number, "1e1");
  verify_advance(&advance_past_number, "1E1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_exp_with_plus) {
  verify_advance(&advance_past_number, "1e+1");
  verify_advance(&advance_past_number, "1E+1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_exp_with_minus) {
  verify_advance(&advance_past_number, "1e-1");
  verify_advance(&advance_past_number, "1E-1");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_number_nonint_exp) {
  verify_advance_partial(&advance_past_number, "1e-1.1", 4);
  verify_advance_partial(&advance_past_number, "1E-1.1", 4);
}

/*
 * Advance past value
 */

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_empty) {
  verify_advance_fail(&advance_past_value, "");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_invalid_character) {
  verify_advance_fail(&advance_past_value, "a");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_string) {
  verify_advance(&advance_past_value, "\"hello\"");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_number) {
  verify_advance(&advance_past_value, "-1.3e+2");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_boolean) {
  verify_advance(&advance_past_value, "true");
  verify_advance(&advance_past_value, "false");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_null) {
  verify_advance(&advance_past_value, "null");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_array) {
  verify_advance(&advance_past_value, "[]");
  verify_advance(&advance_past_value, "[1,null,true]");
  verify_advance(&advance_past_value, "[ 1 , null , true ]");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_object) {
  verify_advance(&advance_past_value, "{}");
  verify_advance(&advance_past_value, R"({"a":3})");
  verify_advance(&advance_past_value, R"({"a":3,"b":4})");
  verify_advance(&advance_past_value, R"({ "a" : 3 , "b" : 4 })");
  verify_advance_fail(&advance_past_value, "{true:false}");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_nested_array) {
  verify_advance(&advance_past_value, "[{},[1],[[1]]]");
  verify_advance(&advance_past_value, "[1,[1],[[1]]]");
}

BOOST_AUTO_TEST_CASE(json_decoding_helpers_advance_past_value_nested_object) {
  verify_advance(&advance_past_value, R"({"a":{}})");
  verify_advance(&advance_past_value, R"({"a":[]})");
  verify_advance(&advance_past_value, R"({"a":[{},[]]})");
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
