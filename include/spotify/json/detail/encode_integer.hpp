/*
 * Copyright (c) 2016 Spotify AB
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

#include <cstdint>

#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

void encode_negative_integer_32(encode_context &context, int32_t value);
void encode_negative_integer_64(encode_context &context, int64_t value);
void encode_positive_integer_32(encode_context &context, uint32_t value);
void encode_positive_integer_64(encode_context &context, uint64_t value);

template <typename T>
json_force_inline void encode_negative_integer(encode_context &context, T value) {
  return (sizeof(T) <= sizeof(int32_t)) ?
    encode_negative_integer_32(context, value) :
    encode_negative_integer_64(context, value);
}

template <typename T>
json_force_inline void encode_positive_integer(encode_context &context, T value) {
  return (sizeof(T) <= sizeof(uint32_t)) ?
    encode_positive_integer_32(context, value) :
    encode_positive_integer_64(context, value);
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
