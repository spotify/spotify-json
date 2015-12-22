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
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

class raw final {
 public:
  struct ref {
    ref() : data(nullptr), size(0) {}
    ref(const char *d, size_t s) : data(d), size(s) {}

    const char *data;
    size_t size;
  };

  using object_type = ref;

  void encode(const object_type &r, writer &w) const {
    w.write(r.data, r.size);
  }

  object_type decode(decoding_context &context) const {
    const auto begin = context.position;
    detail::advance_past_value(context);
    return ref(begin, context.position - begin);
  }
};

}  // namespace codec

template<>
struct default_codec_t<codec::raw::ref> {
  static codec::raw codec() {
    return codec::raw();
  }
};

}  // namespace json
}  // namespace spotify
