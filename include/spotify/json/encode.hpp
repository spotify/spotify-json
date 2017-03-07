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

#include <string>

#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encode_context.hpp>
#include <spotify/json/encoded_value.hpp>

namespace spotify {
namespace json {

template <typename codec_type, typename object_type>
json_never_inline std::string encode(
    const codec_type &codec,
    const object_type &object) {
  encode_context context;
  codec.encode(context, object);
  return std::string(context.data(), context.size());
}

template <typename object_type>
json_never_inline std::string encode(const object_type &object) {
  return encode(default_codec<object_type>(), object);
}

template <typename codec_type, typename value_type>
json_never_inline encoded_value encode_value(
    const codec_type &codec,
    const value_type &value) {
  encode_context context;
  codec.encode(context, value);
  return encoded_value(std::move(context), encoded_value::unsafe_unchecked());
}

template <typename value_type>
json_never_inline encoded_value encode_value(const value_type &value) {
  return encode_value(default_codec<value_type>(), value);
}

}  // namespace json
}  // namespace spotify
