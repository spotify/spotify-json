/*
 * Copyright (c) 2015-2019 Spotify AB
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

#include <spotify/json/codec/any_value.hpp>

#include <spotify/json/detail/skip_value.hpp>

namespace spotify {
namespace json {
namespace codec {

any_value_t::object_type any_value_t::decode(decode_context &context) const {
  const auto begin = context.position;
  detail::skip_value(context);
  const auto size = context.position - begin;
  return object_type(begin, size, object_type::unsafe_unchecked());
}

void any_value_t::encode(encode_context &context, const object_type &value) const {
  context.append(value.data(), value.size());
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
