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

#include <spotify/json/codec/standard.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename T>
class real_t final : public detail::primitive_encoder<T> {
 public:
  object_type decode(decoding_context &context) const;
};

template<typename T>
real_t<T> real() {
  return real_t<T>();
}

template<>
struct standard_t<float> {
  static real_t<float> codec() {
    return real_t<float>();
  }
};

template<>
struct standard_t<double> {
  static real_t<double> codec() {
    return real_t<double>();
  }
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
