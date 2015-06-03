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

#include <cmath>

#include <double-conversion/double-conversion.h>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>
#include <spotify/json/standard.hpp>

namespace spotify {
namespace json {
namespace detail {

template<typename T>
T decode_real(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count);

template<>
float decode_real(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count) {
  return converter.StringToFloat(buffer, length, processed_characters_count);
}

template<>
double decode_real(
    const double_conversion::StringToDoubleConverter &converter,
    const char* buffer,
    int length,
    int* processed_characters_count) {
  return converter.StringToDouble(buffer, length, processed_characters_count);
}

}  // namespace detail

namespace codec {

template<typename T>
class real_t final : public detail::primitive_encoder<T> {
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
    const auto result = detail::decode_real<T>(
        converter,
        context.position,
        context.end - context.position,
        &bytes_read);
    detail::require(context, !std::isnan(result), "Invalid floating point number");
    context.position += bytes_read;
    return result;
  }
};

template<typename T>
real_t<T> real() {
  return real_t<T>();
}

}  // namespace codec

template<>
struct standard_t<float> {
  static codec::real_t<float> codec() {
    return codec::real_t<float>();
  }
};

template<>
struct standard_t<double> {
  static codec::real_t<double> codec() {
    return codec::real_t<double>();
  }
};

}  // namespace json
}  // namespace spotify
