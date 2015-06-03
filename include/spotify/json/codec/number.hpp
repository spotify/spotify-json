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

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>

namespace spotify {
namespace json {
namespace detail {

template<typename T>
T decode_floating_point(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count);

template<>
inline float decode_floating_point(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count) {
  return converter.StringToFloat(buffer, length, processed_characters_count);
}

template<>
inline double decode_floating_point(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count) {
  return converter.StringToDouble(buffer, length, processed_characters_count);
}

template<typename T>
class floating_point_t : public detail::primitive_encoder<T> {
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
    const auto result = detail::decode_floating_point<T>(
        converter,
        context.position,
        context.end - context.position,
        &bytes_read);
    detail::require(context, !std::isnan(result), "Invalid floating point number");
    context.position += bytes_read;
    return result;
  }
};

}  // namespace detail

namespace codec {

template<typename T>
class number_t final : public detail::primitive_encoder<T> {
 public:
  T decode(decoding_context &context) const {
    return T();
  }
};

template<>
class number_t<float> final : public detail::floating_point_t<float> {
};

template<>
class number_t<double> final : public detail::floating_point_t<double> {
};

template<typename T>
number_t<T> number() {
  return number_t<T>();
}

}  // namespace codec

template<typename T>
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
