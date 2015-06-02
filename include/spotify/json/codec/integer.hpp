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

#include <spotify/json/detail/primitive_encoder.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/standard.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename T>
class integer_t final : public detail::primitive_encoder<T> {
 public:
  T decode(decoding_context &context) const {
    return T();
  }
};

template<typename T>
integer_t<T> integer() {
  return integer_t<T>();
}

}  // namespace codec

template<typename T>
struct standard_t {
  static_assert(std::is_integral<T>::value, "No standard_t specialization for type T");

  static codec::integer_t<T> codec() {
    return codec::integer_t<T>();
  }
};

}  // namespace json
}  // namespace spotify
