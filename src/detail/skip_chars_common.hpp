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

#include <spotify/json/detail/macros.hpp>

#define json_unaligned_x(ignore) true

#define JSON_STRING_SKIP_N_SIMPLE(n, n_plus_one, type, control, goto_label) \
  control ((end - pos) >= n && json_unaligned_ ## n_plus_one(pos)) { \
    const auto cc = *reinterpret_cast<const type *>(pos); \
    if (json_haschar_ ## n(cc, '"')) { goto goto_label; } \
    if (json_haschar_ ## n(cc, '\\')) { goto goto_label; } \
    pos += n; \
  }
