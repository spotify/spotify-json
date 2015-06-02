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
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>

namespace spotify {
namespace json {
namespace codec {

class string_t final : public detail::primitive_encoder<std::string> {
 public:
  object_type decode(decoding_context &context) const {
    detail::advance_past(context, '"');
    if (context.has_failed()) {
      return "";
    }
    const char *start = context.position;
    for (;; ++context.position) {
      if (context.position == context.end) {
        context.error = "Unexpected end of input in string";
        return "";
      } else if (*context.position == '"') {
        break;
      }
    }
    return std::string(start, context.position++);
  }
};

inline string_t string() {
  return string_t();
}

template<>
struct standard_t<std::string> {
  static string_t codec() {
    return string_t();
  }
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
