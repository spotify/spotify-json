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

template <typename T, bool is_positive>
json_never_inline T decode_integer(decoding_context &context) {
  using intops = integer_ops<T, is_positive>;
  const auto c = next(context);
  const auto i = static_cast<T>(char_traits<char>::to_integer(c));
  fail_if(context, i < 0 || i > 9, "Invalid integer");
  T value = intops::accumulate(0, i);

  while (context.remaining()) {
    const auto c = next_unchecked(context);
    const auto i = static_cast<T>(char_traits<char>::to_integer(c));
    if (i < 0 || i > 9) {
      // TODO(johanl): If c is '.', skip past the rest of the decimal number.
      break;
    }
    const auto old_value = value;
    value = intops::accumulate(value * 10, i);
    fail_if(context, intops::is_overflow(old_value, value), "Integer overflow");
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
