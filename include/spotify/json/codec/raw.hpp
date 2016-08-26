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

#include <cstring>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace codec {

struct raw_ref {
  raw_ref() : data(nullptr), size(0) {}
  raw_ref(const char *d, size_t s) : data(d), size(s) {}
  raw_ref(const char *begin, const char *end) : data(begin), size(end - begin) {}

  explicit operator decoding_context() const { return decoding_context(data, size); }

  const char *data;
  size_t size;
};

class raw_t final {
 public:
  using object_type = raw_ref;

  object_type decode(decoding_context &context) const {
    const auto begin = context.position;
    detail::advance_past_value(context);
    return raw_ref(begin, context.position - begin);
  }

  void encode(encoding_context &context, const object_type &value) const {
    std::memcpy(context.reserve(value.size), value.data, value.size);
    context.advance(value.size);
  }
};

inline raw_t raw() {
  return raw_t();
}

}  // namespace codec

template <>
struct default_codec_t<codec::raw_ref> {
  static codec::raw_t codec() {
    return codec::raw();
  }
};

}  // namespace json
}  // namespace spotify
