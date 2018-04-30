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
#include <spotify/json/codec/string.hpp>
#include <spotify/json/detail/decode_helpers.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

decode_context make_context(const char *str) {
  return decode_context(str, str + strlen(str));
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

BOOST_AUTO_TEST_CASE(json_decode_helpers_peek_with_empty_input) {
  BOOST_CHECK_EQUAL(peek(make_context("")), '\0');
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_peek_at_last_character) {
  BOOST_CHECK_EQUAL(peek(make_context("a")), 'a');
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_peek) {
  BOOST_CHECK_EQUAL(peek(make_context("ab")), 'a');
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_peek_2) {
  BOOST_CHECK(peek_2(make_context("ab"), 'a', 'b'));
  BOOST_CHECK(peek_2(make_context("abcd"), 'a', 'b'));
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_peek_2_nonmatching) {
  BOOST_CHECK(!peek_2(make_context("aa"), 'a', 'b'));
  BOOST_CHECK(!peek_2(make_context("bb"), 'a', 'b'));
  BOOST_CHECK(!peek_2(make_context("aab"), 'a', 'b'));
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_peek_2_too_short) {
  BOOST_CHECK(!peek_2(make_context(""), 'a', 'b'));
  BOOST_CHECK(!peek_2(make_context("a"), 'a', 'b'));
  BOOST_CHECK(!peek_2(make_context("b"), 'a', 'b'));
}

/*
 * Next
 */

BOOST_AUTO_TEST_CASE(json_decode_helpers_next_with_empty_input_should_fail) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(next(ctx), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_next_at_last_character) {
  auto ctx = make_context("a");
  BOOST_CHECK_EQUAL(next(ctx), 'a');
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_next) {
  auto ctx = make_context("ab");
  BOOST_CHECK_EQUAL(next(ctx), 'a');
  BOOST_CHECK_EQUAL(next(ctx), 'b');
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_next_unchecked_at_last_character) {
  auto ctx = make_context("a");
  BOOST_CHECK_EQUAL(next_unchecked(ctx), 'a');
  BOOST_CHECK(!ctx.remaining());
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_next_unchecked) {
  auto ctx = make_context("ab");
  BOOST_CHECK_EQUAL(next_unchecked(ctx), 'a');
  BOOST_CHECK_EQUAL(next_unchecked(ctx), 'b');
  BOOST_CHECK(!ctx.remaining());
}

/*
 * skip_any_1
 */

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_with_empty_input_should_fail) {
  auto context = make_context("");
  BOOST_CHECK_THROW(skip_any_1(context), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_at_last_character) {
  auto context = make_context("a");
  skip_any_1(context);
  BOOST_CHECK(!context.remaining());
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip) {
  auto context = make_context("ab");
  skip_any_1(context);
  skip_any_1(context);
  BOOST_CHECK(!context.remaining());
}

/*
 * Advance past whitespace
 */

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_any_whitespace_with_empty_input) {
  auto ctx = make_context("");
  const auto original_ctx = ctx;
  skip_any_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_any_whitespace_with_non_whitespace_input) {
  auto ctx = make_context("a");
  const auto original_ctx = ctx;
  skip_any_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_any_whitespace_with_whitespace_input_to_end) {
  auto ctx = make_context(" \t\r\n");
  const auto original_ctx = ctx;
  skip_any_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_any_whitespace_with_whitespace_input) {
  auto ctx = make_context(" a\t\r\n");
  const auto original_ctx = ctx;
  skip_any_whitespace(ctx);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 1);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

/*
 * Skip 1
 */

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_1_with_empty_input) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(skip_1(ctx, 'a'), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_1_with_matching_input_to_end) {
  auto ctx = make_context("a");
  const auto original_ctx = ctx;
  skip_1(ctx, 'a');

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_1_with_matching_input) {
  auto ctx = make_context("aaa");
  const auto original_ctx = ctx;
  skip_1(ctx, 'a');

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 1);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_1_with_nonmatching_input) {
  auto ctx = make_context("b");
  BOOST_CHECK_THROW(skip_1(ctx, 'a'), decode_exception);
}

/*
 * Skip 4
 */

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_4_word_with_empty_input) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(skip_4(ctx, "aaaa"), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_4_word_with_too_short_input) {
  auto ctx = make_context("abc");
  BOOST_CHECK_THROW(skip_4(ctx, "abcd"), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_4_word_with_matching_input_to_end) {
  auto ctx = make_context("abcd");
  const auto original_ctx = ctx;
  skip_4(ctx, "abcd");

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_4_word_with_matching_input) {
  auto ctx = make_context("abcde");
  const auto original_ctx = ctx;
  skip_4(ctx, "abcd");

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 4);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_skip_4_word_with_nonmatching_input) {
  auto ctx = make_context("abcD");
  BOOST_CHECK_THROW(skip_4(ctx, "abcd"), decode_exception);
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
  decode_comma_separated(ctx, '<', '>', [&]{
    skip_1(ctx, expected_char++);
  });

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_empty_input) {
  auto ctx = make_context("");
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', &dont_call), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_wrong_first_character) {
  auto ctx = make_context(">");
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', &dont_call), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_immediate_end) {
  auto ctx = make_context("<>");
  const auto original_ctx = ctx;
  decode_comma_separated(ctx, '<', '>', &dont_call);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_before_first) {
  auto ctx = make_context(" <>");
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', &dont_call), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_after_first) {
  auto ctx = make_context("< >");
  const auto original_ctx = ctx;
  decode_comma_separated(ctx, '<', '>', &dont_call);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_after_last) {
  auto ctx = make_context("<> ");
  const auto original_ctx = ctx;
  decode_comma_separated(ctx, '<', '>', &dont_call);

  BOOST_CHECK_EQUAL(ctx.position, original_ctx.position + 2);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_single_element) {
  parse("<a>");
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_two_elements) {
  parse("<a,b>");
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_before_first_element) {
  parse("< a,b>");
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_before_comma) {
  parse("<a ,b>");
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_after_comma) {
  parse("<a, b>");
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_whitespace_before_last) {
  parse("<a,b >");
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_wrong_last_character) {
  auto ctx = make_context("<<");
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', [&]{}), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_missing_last_character) {
  auto ctx = make_context("<");
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', [&]{}), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_trailing_comma) {
  auto ctx = make_context("<a,>");
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', [&]{
    skip_1(ctx, 'a');
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_comma_separated_with_failing_inner_parse) {
  auto ctx = make_context("<a,a>");
  bool was_called_already = false;
  BOOST_CHECK_THROW(decode_comma_separated(ctx, '<', '>', [&]{
    BOOST_CHECK(!was_called_already);
    was_called_already = true;
    skip_1(ctx, 'b');
  }), decode_exception);
}

/*
 * Advance past object
 */

namespace {

bool decode_boolean(decode_context &context) {
  return codec::boolean().decode(context);
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_empty_object) {
  auto ctx = make_context("{}");
  const auto original_ctx = ctx;
  decode_object<codec::omit_t<bool>>(ctx, [&](bool &&key) {
    BOOST_CHECK(!"Should not be called");
  });
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_with_single_value) {
  auto ctx = make_context("{true:false}");
  const auto original_ctx = ctx;
  bool has_been_called_already = false;
  decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    const auto value = decode_boolean(ctx);
    BOOST_CHECK(!has_been_called_already);
    has_been_called_already = true;
    BOOST_CHECK_EQUAL(key, true);
    BOOST_CHECK_EQUAL(value, false);
  });
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_with_two_values) {
  auto ctx = make_context("{true:false,false:true}");
  const auto original_ctx = ctx;
  size_t times_called = 0;
  decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    const auto value = decode_boolean(ctx);
    BOOST_CHECK_EQUAL(key, !times_called);
    BOOST_CHECK_EQUAL(value, !!times_called);
    times_called++;
  });
  BOOST_CHECK_EQUAL(times_called, 2);
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_with_whitespace) {
  auto ctx = make_context("{ true : false , false : true }");
  const auto original_ctx = ctx;
  size_t times_called = 0;
  decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    const auto value = decode_boolean(ctx);
    BOOST_CHECK_EQUAL(key, !times_called);
    BOOST_CHECK_EQUAL(value, !!times_called);
    times_called++;
  });
  BOOST_CHECK_EQUAL(times_called, 2);
  BOOST_CHECK(ctx.position == original_ctx.end);
  BOOST_CHECK_EQUAL(ctx.end, original_ctx.end);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_with_broken_key) {
  auto ctx = make_context("{tru:false}");
  BOOST_CHECK_THROW(decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    BOOST_CHECK(!"Should not be called");
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_with_broken_value) {
  auto ctx = make_context("{true:fals}");
  BOOST_CHECK_THROW(decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    decode_boolean(ctx);
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_without_colon) {
  auto ctx = make_context("{truefalse}");
  BOOST_CHECK_THROW(decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    BOOST_CHECK(!"Should not be called");
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_without_ending_brace) {
  auto ctx = make_context("{true:false");
  BOOST_CHECK_THROW(decode_object<codec::boolean_t>(ctx, [&](bool &&key) {
    decode_boolean(ctx);
  }), decode_exception);
}

BOOST_AUTO_TEST_CASE(json_decode_helpers_decode_object_with_null_bytes) {
  alignas(16) char input_data[43] =
    "{           \"AAA"
    "BBB\0\":true,    \""
    "CCC\":true}";
  BOOST_REQUIRE(input_data[sizeof(input_data) - 2] == '}');
  auto ctx = decode_context(input_data, sizeof(input_data));
  int num = 0;
  decode_object<codec::string_t>(ctx, [&](std::string &&key) {
    BOOST_CHECK(num < 2);
    if (num == 0) {
      BOOST_CHECK_EQUAL(key, std::string("AAABBB\0", 7));
    } else {
      BOOST_CHECK_EQUAL(key, "CCC");
    }
    num++;
    decode_boolean(ctx);
  });
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
