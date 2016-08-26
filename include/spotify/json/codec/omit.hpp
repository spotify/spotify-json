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

#include <stdexcept>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/encoding_helpers.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T>
class omit_t final {
 public:
  using object_type = T;

  object_type decode(decoding_context &context) const {
    detail::fail(context, "omit_t codec cannot decode");
  }

  void encode(encoding_context &context, const object_type &value) const {
    detail::fail(context, "omit_t codec cannot encode");
  }

  bool should_encode(const object_type &value) const {
    return false;
  }
};

template <typename T>
omit_t<T> omit() {
  return omit_t<T>();
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
