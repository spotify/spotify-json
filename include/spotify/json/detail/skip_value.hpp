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

#pragma once

#include <spotify/json/decode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

/**
 * Skip past one JSON value. If parsing fails, context will be set to that it
 * has failed. If parsing suceeds, context.position will point to the character
 * after the last character of the JSON object that was parsed.
 *
 * context.has_failed() must be false when this function is called.
 */
void skip_value(decode_context &context);

}  // namespace detail
}  // namespace json
}  // namespace spotify
