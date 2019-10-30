/*
 * Copyright (c) 2014-2019 Spotify AB
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

#include <spotify/json/decode_context.hpp>

namespace spotify {
namespace json {

decode_context::decode_context(const char *begin, const char *end)
    : has_sse42(detail::cpuid().has_sse42()),
      position(begin),
      begin(begin),
      end(end) {}

decode_context::decode_context(const char *data, size_t size)
    : has_sse42(detail::cpuid().has_sse42()),
      position(data),
      begin(data),
      end(data + size) {}

}  // namespace json
}  // namespace spotify
