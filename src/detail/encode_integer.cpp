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

#include <spotify/json/detail/encode_integer.hpp>

namespace spotify {
namespace json {
namespace detail {
namespace {

template <typename T, int num_digits>
struct negative final {
  json_force_inline static void encode(encode_context &context, T value) {
    constexpr auto num_bytes = num_digits + 1;  // + 1 for the '-' sign character
    const auto p = context.reserve(num_bytes);
    p[0] = '-';
    for (int i = num_digits; i >= 1; i--) {
      const auto v = value;
      value /= 10;
      p[i] = ('0' + static_cast<uint8_t>(value * 10 - v));
    }
    context.advance(num_bytes);
  }
};

template <typename T, int num_digits>
struct positive final {
  json_force_inline static void encode(encode_context &context, T value) {
    constexpr auto num_bytes = num_digits;
    const auto p = context.reserve(num_bytes);
    for (int i = num_digits - 1; i >= 0; i--) {
      const auto v = value;
      value /= 10;
      p[i] = ('0' + static_cast<uint8_t>(v - value * 10));
    }
    context.advance(num_bytes);
  }
};

}  // namespace

void encode_negative_integer_32(encode_context &context, int32_t value) {
  #define ENCODE(bits, digits) do { \
    using type = int_fast ## bits ## _t; \
    return negative<type, digits>::encode(context, type(value)); \
  } while (false)
  #define CUTOFF(bits, digits, cutoff) if (value > cutoff ## LL) ENCODE(bits, digits);
  CUTOFF(32,  1, -10);
  CUTOFF(32,  2, -100);
  CUTOFF(32,  3, -1000);
  CUTOFF(32,  4, -10000);
  CUTOFF(32,  5, -100000);
  CUTOFF(32,  6, -1000000);
  CUTOFF(32,  7, -10000000);
  CUTOFF(32,  8, -100000000);
  CUTOFF(32,  9, -1000000000);
  ENCODE(32, 10);
  #undef CUTOFF
  #undef ENCODE
}

void encode_negative_integer_64(encode_context &context, int64_t value) {
  #define ENCODE(bits, digits) do { \
    using type = int_fast ## bits ## _t; \
    return negative<type, digits>::encode(context, type(value)); \
  } while (false)
  #define CUTOFF(bits, digits, cutoff) if (value > cutoff ## LL) ENCODE(bits, digits);
  CUTOFF(32,  1, -10);
  CUTOFF(32,  2, -100);
  CUTOFF(32,  3, -1000);
  CUTOFF(32,  4, -10000);
  CUTOFF(32,  5, -100000);
  CUTOFF(32,  6, -1000000);
  CUTOFF(32,  7, -10000000);
  CUTOFF(32,  8, -100000000);
  CUTOFF(32,  9, -1000000000);
  CUTOFF(64, 10, -10000000000);
  CUTOFF(64, 11, -100000000000);
  CUTOFF(64, 12, -1000000000000);
  CUTOFF(64, 13, -10000000000000);
  CUTOFF(64, 14, -100000000000000);
  CUTOFF(64, 15, -1000000000000000);
  CUTOFF(64, 16, -10000000000000000);
  CUTOFF(64, 17, -100000000000000000);
  CUTOFF(64, 18, -1000000000000000000);
  ENCODE(64, 19);
  #undef CUTOFF
  #undef ENCODE
}

void encode_positive_integer_32(encode_context &context, uint32_t value) {
  #define ENCODE(bits, digits) do { \
    using type = uint_fast ## bits ## _t; \
    return positive<type, digits>::encode(context, type(value)); \
  } while (false)
  #define CUTOFF(bits, digits, cutoff) if (value < cutoff ## UL) ENCODE(bits, digits);
  CUTOFF(32,  1, 10);
  CUTOFF(32,  2, 100);
  CUTOFF(32,  3, 1000);
  CUTOFF(32,  4, 10000);
  CUTOFF(32,  5, 100000);
  CUTOFF(32,  6, 1000000);
  CUTOFF(32,  7, 10000000);
  CUTOFF(32,  8, 100000000);
  CUTOFF(32,  9, 1000000000);
  ENCODE(32, 10);
  #undef CUTOFF
  #undef ENCODE
}

void encode_positive_integer_64(encode_context &context, uint64_t value) {
  #define ENCODE(bits, digits) do { \
    using type = uint_fast ## bits ## _t; \
    return positive<type, digits>::encode(context, type(value)); \
  } while (false)
  #define CUTOFF(bits, digits, cutoff) if (value < cutoff ## ULL) ENCODE(bits, digits);
  CUTOFF(32,  1, 10);
  CUTOFF(32,  2, 100);
  CUTOFF(32,  3, 1000);
  CUTOFF(32,  4, 10000);
  CUTOFF(32,  5, 100000);
  CUTOFF(32,  6, 1000000);
  CUTOFF(32,  7, 10000000);
  CUTOFF(32,  8, 100000000);
  CUTOFF(32,  9, 1000000000);
  CUTOFF(64, 10, 10000000000);
  CUTOFF(64, 11, 100000000000);
  CUTOFF(64, 12, 1000000000000);
  CUTOFF(64, 13, 10000000000000);
  CUTOFF(64, 14, 100000000000000);
  CUTOFF(64, 15, 1000000000000000);
  CUTOFF(64, 16, 10000000000000000);
  CUTOFF(64, 17, 100000000000000000);
  CUTOFF(64, 18, 1000000000000000000);
  CUTOFF(64, 19, 10000000000000000000);
  ENCODE(64, 20);
  #undef CUTOFF
  #undef ENCODE
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
