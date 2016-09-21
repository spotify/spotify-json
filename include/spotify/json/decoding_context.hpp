/*
 * Copyright (c) 2014-2016 Spotify AB
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

#include <cstddef>

#include <spotify/json/decode_exception.hpp>
#include <spotify/json/detail/cpuid.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

/**
 * A decoding_context has the information that is kept while decoding JSON with
 * codecs. It has information about the data to read and whether the decoding
 * has failed.
 */
struct decoding_context final {
  decoding_context(const char *begin, const char *end)
      : has_sse42(detail::cpuid().has_sse42()),
        position(begin),
        begin(begin),
        end(end) {}

  decoding_context(const char *data, size_t size)
      : has_sse42(detail::cpuid().has_sse42()),
        position(data),
        begin(data),
        end(data + size) {}

  json_force_inline size_t offset() const {
    return (position - begin);
  }

  json_force_inline size_t offset(const ptrdiff_t d) const {
    return offset() + d;
  }

  json_force_inline size_t remaining() const {
    return (end - position);
  }

  const bool has_sse42;
  const char *position;
  const char *const begin;
  const char *const end;
};

}  // namespace json
}  // namespace spotify
