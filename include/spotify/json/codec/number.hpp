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

#include <algorithm>
#include <limits>
#include <type_traits>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename T>
T decode_floating_point(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count);

template <>
inline float decode_floating_point(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count) {
  return converter.StringToFloat(buffer, length, processed_characters_count);
}

template <>
inline double decode_floating_point(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count) {
  return converter.StringToDouble(buffer, length, processed_characters_count);
}

template <typename T>
class floating_point_t : public primitive_encoder<T> {
 public:
  T decode(decoding_context &context) const {
    using atod_converter = double_conversion::StringToDoubleConverter;
    static const atod_converter converter(
        atod_converter::ALLOW_TRAILING_JUNK,
        std::numeric_limits<T>::quiet_NaN(),
        std::numeric_limits<T>::quiet_NaN(),
        nullptr,
        nullptr);

    int bytes_read = 0;
    const auto result = decode_floating_point<T>(
        converter,
        context.position,
        context.end - context.position,
        &bytes_read);
    fail_if(context, std::isnan(result), "Invalid floating point number");
    skip(context, bytes_read);
    return result;
  }
};

template <typename T, bool is_positive>
struct integer_ops {
  static json_force_inline bool is_overflow(T old_value, T new_value) {
    return (new_value < old_value);
  }

  static json_force_inline T accumulate(T old_value, T acc_value) {
    return (old_value + acc_value);
  }
};

template <typename T>
struct integer_ops<T, false> {
  static json_force_inline bool is_overflow(T old_value, T new_value) {
    return (new_value > old_value);
  }

  static json_force_inline T accumulate(T old_value, T acc_value) {
    return (old_value - acc_value);
  }
};

json_force_inline bool is_invalid_digit(int digit) {
  const auto forced_positive = static_cast<unsigned>(digit);
  return (forced_positive > 9);  // negative number -> very large positive number
}

/**
 * Calculate 'exp_10(e, v) = v * 10^e', throwing a decode_exception if the value
 * overflows the integer type. This function executes in linear time over the
 * value of 'e', so it will not be very efficient for large exponents, although
 * the value will overflow rather quickly so the runtime is bounded (a value of
 * zero is specifically handled to avoid a semi-infinite loop). Note that the
 * argument are reversed, to make the function easier to call from the functions
 * below (only decode_with_positive_exponent_and_too_few_decimal_digits(...)).
 */
template <typename T, bool is_positive>
json_force_inline T exp_10(
    decoding_context &context,
    const unsigned exponent,
    const T initial_value) {
  T value = initial_value;
  if (json_likely(value)) {
    using intops = integer_ops<T, is_positive>;
    for (unsigned i = 0; i < exponent; i++) {
      const auto old_value = value;
      value *= 10;
      fail_if(context, intops::is_overflow(old_value, value), "Integer overflow");
    }
  }
  return value;
}

/**
 * Decode an integer specified in a byte range. The range must be known to only
 * contain digits characters ('0' through '9'). If it contains anything else,
 * the result is undefined. The output variable 'did_overflow' will be set when
 * the parsed value overflows the integer type.
 */
template <typename T, bool is_positive>
json_never_inline T decode_integer_range_with_overflow(
    decoding_context &context,
    const char *begin,
    const char *end,
    const T initial_value,
    bool &did_overflow) {
  T value = initial_value;

  using intops = integer_ops<T, is_positive>;
  for (auto it = begin; it != end; ++it) {
    const auto c = (*it);
    const auto i = char_traits<char>::to_integer(c);
    const auto old_value = value;
    value = intops::accumulate(value * 10, i);
    if (json_unlikely(intops::is_overflow(old_value, value))) {
      did_overflow = true;
      return old_value;
    }
  }

  return value;
}

/**
 * Decode an integer specified in a byte range. The range must be known to only
 * contain digits characters ('0' through '9'). If it contains anything else,
 * the result is undefined. A decode_exception is thrown if the value overflows
 * the integer type.
 */
template <typename T, bool is_positive>
json_never_inline T decode_integer_range(
    decoding_context &context,
    const char *begin,
    const char *end,
    const T initial_value = 0) {
  bool did_overflow = false;
  const T value = decode_integer_range_with_overflow<T, is_positive>(
      context,
      begin,
      end,
      initial_value,
      did_overflow);
  fail_if(context, did_overflow, "Integer overflow");
  return value;
}

/**
 * Decode an integer that has a negative exponent. We can easily do this by
 * cutting off the least significant digits of the integer part of the number.
 * If the negative exponent is larger than the number of integer digits, zero is
 * returned. If the parsed number is too large to fit in the given integer type,
 * a decode_exception is thrown.
 */
template <typename T, bool is_positive>
json_never_inline T decode_with_negative_exponent(
    decoding_context &context,
    const unsigned exponent,
    const char *int_beg,
    const char *int_end) {
  const auto num_int_digits = (int_end - int_beg);
  const auto lshift_int_end = (int_end - exponent);
  return (json_likely(num_int_digits > exponent) ?
      decode_integer_range<T, is_positive>(context, int_beg, lshift_int_end) :
      0);  // the negative exponent is larger than the number of digits, nothing left
}

/**
 * Decode an integer that has a positive exponent. We can easily do this by
 * pretending that the decimal digits are just a continuation of the integer
 * digits, until the exponent has been used up. If there are not enough decimal
 * digits, the remaining exponent is used to multiply the parsed (from both the
 * integer and decimal digits) value appropriately. If the parsed number is too
 * large to fit in the given integer type, a decode_exception is thrown.
 */
template <typename T, bool is_positive>
json_never_inline T decode_with_positive_exponent(
    decoding_context &context,
    const unsigned exponent,
    const char *int_beg,
    const char *int_end,
    const char *dec_beg,
    const char *dec_end) {
  T value;
  const auto num_dec_digits = (dec_end - dec_beg);
  if (num_dec_digits >= exponent) {
    value = decode_integer_range<T, is_positive>(context, int_beg, int_end);
    value = decode_integer_range<T, is_positive>(context, dec_beg, dec_beg + exponent, value);
  } else {
    value = decode_integer_range<T, is_positive>(context, int_beg, int_end);
    value = decode_integer_range<T, is_positive>(context, dec_beg, dec_end, value);
    value = exp_10<T, is_positive>(context, exponent - num_dec_digits, value);
  }
  return value;
}

/**
 * It is possible that the exponent overflows, but this does not necessary mean
 * that the parsed integer would overflow as well, e.g., a zero integer with a
 * very large exponent is still a zero integer; so when we catch an overflow
 * error for the exponent, we do some special case handling to figure out which
 * integer value to return.
 */
template <typename T>
json_never_inline T handle_overflowing_exponent(
    decoding_context &context,
    const bool exp_is_positive,
    const char *int_beg,
    const char *int_end,
    const char *dec_beg,
    const char *dec_end) {
  bool ignore;
  const auto i = decode_integer_range_with_overflow<unsigned, true>(context, int_beg, int_end, 0, ignore);
  const auto d = decode_integer_range_with_overflow<unsigned, true>(context, dec_beg, dec_end, 0, ignore);
  fail_if(context, exp_is_positive && (i || d), "Integer overflow");
  return 0;
}

/**
 * Decode the "tricky" integer at the given context position. By tricky we mean
 * an integer that has decimal digits or an exponent or both. This function is
 * carefully constructed to not overflow unless the parsed integer (taking the
 * exponent into account) overflows the given type, e.g., 52e-1 = 5. It also
 * makes sure to not discard the decimal digits until the exponent has been
 * taken into account, e.g., 5.2e1 = 52. If the parsed number is too large to
 * fit in the given integer type, a decode_exception is thrown.
 */
template <typename T, bool is_positive>
json_never_inline T decode_integer_tricky(decoding_context &context, const char *int_beg) {
  // Find [xxxx].yyyyE±zzzz
  auto int_end = std::find_if_not(int_beg, context.end, char_traits<char>::is_digit);
  context.position = int_end;

  // Find xxxx.[yyyy]E±zzzz
  decltype(context.position) dec_beg = nullptr;
  decltype(context.position) dec_end = nullptr;
  if (peek(context) == '.') {
    skip(context);
    dec_beg = context.position;
    dec_end = std::find_if_not(dec_beg, context.end, char_traits<char>::is_digit);
    fail_if(context, dec_beg == dec_end, "Invalid digits after decimal point");
    context.position = dec_end;
  }

  // Find xxxx.yyyyE[±zzzz]
  auto exp_is_positive = true;
  decltype(context.position) exp_beg = nullptr;
  decltype(context.position) exp_end = nullptr;
  const auto e = peek(context);
  if (e == 'e' || e == 'E') {
    skip(context);
    const auto sign = peek(context);
    if (sign == '-' || sign == '+') {
      exp_is_positive = (sign == '+');
      skip(context);
    }
    exp_beg = context.position;
    exp_end = std::find_if_not(exp_beg, context.end, char_traits<char>::is_digit);
    fail_if(context, exp_beg == exp_end, "Exponent symbols should be followed by an optional '+' or '-' and then by at least one number");
    context.position = exp_end;
  }

  bool did_overflow = false;
  const auto exp = decode_integer_range_with_overflow<unsigned, true>(context, exp_beg, exp_end, 0, did_overflow);
  if (json_unlikely(did_overflow)) {
    return handle_overflowing_exponent<T>(context, exp_is_positive, int_beg, int_end, dec_beg, dec_end);
  }

  return (json_likely(exp_is_positive) ?
      decode_with_positive_exponent<T, is_positive>(context, exp, int_beg, int_end, dec_beg, dec_end) :
      decode_with_negative_exponent<T, is_positive>(context, exp, int_beg, int_end));
}

/**
 * Decode the integer at the context position. The integer can be specified
 * either as a pure integer: 'xxxx', where 'x' is a digit character between '0'
 * and '9'; and 'xxxx.yyyyE±zzzz', where 'y' and 'z' also are digit characters.
 * Pure integers are easy and fast to parse, but if we run into a decimal point
 * or an "exponent E", we need to switch over to a more complex parser. The same
 * thing happens if we overflow, because we cannot yet know if this was a true
 * overflow or if a negative exponent will reduce the integer into range again.
 * If the parsed number is too large to fit in the given integer type, a
 * decode_exception is thrown. Decimal digits are simply discarded if they are
 * not used, i.e., if there is no positive exponent.
 */
template <typename T, bool is_positive>
json_never_inline T decode_integer(decoding_context &context) {
  using intops = integer_ops<T, is_positive>;
  const auto b = context.position;
  const auto c = next(context);
  const auto i = char_traits<char>::to_integer(c);
  fail_if(context, is_invalid_digit(i), "Invalid integer");
  T value = intops::accumulate(0, i);

  while (json_likely(context.remaining())) {
    const auto c = next_unchecked(context);
    const auto i = static_cast<T>(char_traits<char>::to_integer(c));
    if (is_invalid_digit(i)) {
      const auto is_tricky = (c == '.' | c == 'e' | c == 'E');
      return (json_unlikely(is_tricky) ? decode_integer_tricky<T, is_positive>(context, b) : value);
    }

    const auto old_value = value;
    value = intops::accumulate(value * 10, i);
    if (json_unlikely(intops::is_overflow(old_value, value))) {
      return decode_integer_tricky<T, is_positive>(context, b);
    }
  }

  return value;
}

template <typename T>
json_force_inline T decode_negative_integer(decoding_context &context) {
  skip(context);  // Skip past leading '-' character (checked in decode(...)).
  return decode_integer<T, false>(context);
}

template <typename T>
json_force_inline T decode_positive_integer(decoding_context &context) {
  return decode_integer<T, true>(context);
}

template <typename T, bool is_integer, bool is_signed>
class integer_t;

template <typename T>
class integer_t<T, true, false> : public primitive_encoder<T> {
 public:
  json_force_inline T decode(decoding_context &context) const {
    return decode_positive_integer<T>(context);
  }
};

template <typename T>
class integer_t<T, true, true> : public primitive_encoder<T> {
 public:
  json_force_inline T decode(decoding_context &context) const {
    return (peek(context) == '-' ?
        decode_negative_integer<T>(context) :
        decode_positive_integer<T>(context));
  }
};

}  // namespace detail

namespace codec {

template <typename T>
class number_t final : public detail::integer_t<T,
    std::is_integral<T>::value,
    std::is_signed<T>::value> {
};

template <>
class number_t<float> final : public detail::floating_point_t<float> {
};

template <>
class number_t<double> final : public detail::floating_point_t<double> {
};

template <typename T>
number_t<T> number() {
  return number_t<T>();
}

}  // namespace codec

template <typename T>
struct default_codec_t {
  static_assert(
      std::is_integral<T>::value || std::is_floating_point<T>::value,
      "No default_codec_t specialization for type T");

  static codec::number_t<T> codec() {
    return codec::number_t<T>();
  }
};

}  // namespace json
}  // namespace spotify
