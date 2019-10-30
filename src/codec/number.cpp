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

#include <spotify/json/codec/number.hpp>

#include <limits>
#include <double-conversion/double-conversion.h>

namespace spotify {
namespace json {
namespace detail {

float decode_float(decode_context &context) {
  using atod_converter = double_conversion::StringToDoubleConverter;
  static const atod_converter converter(
      atod_converter::ALLOW_TRAILING_JUNK,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      nullptr,
      nullptr);

  int bytes_read = 0;
  auto remaining = static_cast<int>(context.end - context.position);
  auto result = converter.StringToFloat(context.position, remaining, &bytes_read);
  fail_if(context, std::isnan(result), "Invalid floating point number");
  skip_unchecked_n(context, bytes_read);
  return result;
}

double decode_double(decode_context &context) {
  using atod_converter = double_conversion::StringToDoubleConverter;
  static const atod_converter converter(
      atod_converter::ALLOW_TRAILING_JUNK,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      nullptr,
      nullptr);

  int bytes_read = 0;
  auto remaining = static_cast<int>(context.end - context.position);
  auto result = converter.StringToDouble(context.position, remaining, &bytes_read);
  fail_if(context, std::isnan(result), "Invalid floating point number");
  skip_unchecked_n(context, bytes_read);
  return result;
}

void encode_float(encode_context &context, float value) {
  // The maximum buffer size required to emit a double in base 10, for decimal
  // and exponential representations, is 25 bytes; based on the settings used
  // below for the DoubleToStringConverter. We add another byte for the null
  // terminator, but it is not actually needed because we don't finalize the
  // builder.
  const auto max_required_size = 26;
  const auto p = reinterpret_cast<char *>(context.reserve(max_required_size));

  // The converter is based on the ECMAScript converter, but will not convert
  // special values, like Infinity and NaN, since JSON does not support those.
  using dtoa_converter = double_conversion::DoubleToStringConverter;
  const dtoa_converter converter(
      dtoa_converter::UNIQUE_ZERO | dtoa_converter::EMIT_POSITIVE_EXPONENT_SIGN,
      nullptr, nullptr, 'e', -6, 21, 6, 0);

  using dtoa_builder = double_conversion::StringBuilder;
  dtoa_builder builder(p, max_required_size);
  detail::fail_if(context, !converter.ToShortest(value, &builder), "Special values like 'Infinity' or 'NaN' are supported in JSON.");
  context.advance(builder.position());
}

void encode_double(encode_context &context, double value) {
  // The maximum buffer size required to emit a double in base 10, for decimal
  // and exponential representations, is 25 bytes; based on the settings used
  // below for the DoubleToStringConverter. We add another byte for the null
  // terminator, but it is not actually needed because we don't finalize the
  // builder.
  const auto max_required_size = 26;
  const auto p = reinterpret_cast<char *>(context.reserve(max_required_size));

  // The converter is based on the ECMAScript converter, but will not convert
  // special values, like Infinity and NaN, since JSON does not support those.
  using dtoa_converter = double_conversion::DoubleToStringConverter;
  const dtoa_converter converter(
      dtoa_converter::UNIQUE_ZERO | dtoa_converter::EMIT_POSITIVE_EXPONENT_SIGN,
      nullptr, nullptr, 'e', -6, 21, 6, 0);

  using dtoa_builder = double_conversion::StringBuilder;
  dtoa_builder builder(p, max_required_size);
  detail::fail_if(context, !converter.ToShortest(value, &builder), "Special values like 'Infinity' or 'NaN' are supported in JSON.");
  context.advance(builder.position());
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
