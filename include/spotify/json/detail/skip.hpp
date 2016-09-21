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

#include <spotify/json/decoding_context.hpp>

namespace spotify {
namespace json {
namespace detail {

/**
 * Skip past the bytes of the string until either a " or a \ character is
 * found. This method attempts to skip as large chunks of memory as possible
 * at each step, by making sure that the context position is aligned to the
 * appropriate address and then reading and comparing several bytes in a
 * single read operation.
 */
void skip_past_simple_characters(decoding_context &context);

/**
 * Skip past the bytes of the string until a non-whitespace character is
 * found. This method attempts to skip as large chunks of memory as possible
 * at each step, by making sure that the context position is aligned to the
 * appropriate address and then reading and comparing several bytes in a
 * single read operation.
 */
void skip_past_whitespace(decoding_context &context);

}  // detail
}  // json
}  // spotify
