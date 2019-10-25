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

#pragma once

#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

/**
 * \brief Escape a string for use in a JSON string as per RFC 4627.
 *
 * This escapes control characters (0x00 through 0x1F), as well as
 * backslashes and quotation marks.
 *
 * See: http://www.ietf.org/rfc/rfc4627.txt (Section 2.5)
 */
void write_escaped(encode_context &context, const char *begin, const char *end);

}  // namespace detail
}  // namespace json
}  // namespace spotify
