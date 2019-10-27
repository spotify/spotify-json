/*
 * Copyright (c) 2016-2019 Spotify AB
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

#include <spotify/json/encoded_value.hpp>

#include <algorithm>
#include <limits>
#include <spotify/json/detail/cpuid.hpp>

namespace spotify {
namespace json {
namespace detail {

void encoded_value_base::validate_json(const char *data, std::size_t size) {
  decode_context context(data, size);
  detail::skip_value(context);  // validate provided JSON string
  detail::fail_if(context, context.position != context.end, "Unexpected trailing input");
}

}  // namespace detail

}  // namespace json
}  // namespace spotify
