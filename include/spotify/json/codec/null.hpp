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
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>
#include <spotify/json/standard.hpp>

namespace spotify {
namespace json {
namespace codec {

class null_t final : public detail::primitive_encoder<null_type> {
 public:
  object_type decode(decoding_context &context) const {
    detail::advance_past_null(context);
    return null_type();
  }
};

inline null_t null() {
  return null_t();
}

}  // namespace codec

template<>
struct standard_t<null_type> {
  static codec::null_t codec() {
    return codec::null_t();
  }
};

}  // namespace json
}  // namespace spotify
