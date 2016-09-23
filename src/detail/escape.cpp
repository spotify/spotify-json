/*
 * Copyright (c) 2015-2016 Spotify AB
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

#include <spotify/json/detail/escape.hpp>

#include <cstring>

#include "escape_common.hpp"

namespace spotify {
namespace json {
namespace detail {

void write_escaped_scalar(
    encode_context &context,
    const uint8_t *begin,
    const uint8_t *end) {
  const auto buf = context.reserve(6 * (end - begin));  // 6 is the length of \u00xx
  auto ptr = buf;

  if (json_unaligned_2(begin) && (end - begin) >= 1) { write_escaped_1(ptr, begin); }
  if (json_unaligned_4(begin) && (end - begin) >= 2) { write_escaped_2(ptr, begin); }
  if (json_unaligned_8(begin) && (end - begin) >= 4) { write_escaped_4(ptr, begin); }
  while ((end - begin) >= 8) { write_escaped_8(ptr, begin); }
  if    ((end - begin) >= 4) { write_escaped_4(ptr, begin); }
  if    ((end - begin) >= 2) { write_escaped_2(ptr, begin); }
  if    ((end - begin) >= 1) { write_escaped_1(ptr, begin); }

  context.advance(ptr - buf);
}

}  // detail
}  // json
}  // spotify
