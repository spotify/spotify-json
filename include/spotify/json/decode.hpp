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

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

template <typename Codec>
typename Codec::object_type decode(const Codec &codec, const char *data, size_t size) {
  decode_context c(data, data + size);
  detail::skip_past_whitespace(c);
  const auto result = codec.decode(c);
  detail::skip_past_whitespace(c);
  detail::fail_if(c, c.position != c.end, "Unexpected trailing input");
  return result;
}

template <typename Codec>
typename Codec::object_type decode(const Codec &codec, const std::string &string) {
  return decode(codec, string.data(), string.size());
}

template <typename Value>
Value decode(const char *data, size_t size) {
  return decode(default_codec<Value>(), data, size);
}

template <typename Value>
Value decode(const std::string &string) {
  return decode(default_codec<Value>(), string);
}

template <typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const char *data,
    size_t size) {
  try {
    object = decode(codec, data, size);
    return true;
  } catch (const decode_exception &) {
    return false;
  }
}

template <typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const std::string &string) {
  return try_decode(object, codec, string.data(), string.size());
}

template <typename Value>
bool try_decode(Value &object, const std::string &string) {
  return try_decode(object, default_codec<Value>(), string);
}

template <typename Value>
bool try_decode(Value &object, const char *data, size_t size) {
  return try_decode(object, default_codec<Value>(), data, size);
}

template <typename Codec>
bool try_decode_partial(
    typename Codec::object_type &object,
    const Codec &codec,
    const decode_context &context) {
  try {
    decode_context c(context);
    detail::skip_past_whitespace(c);
    object = codec.decode(c);
    return true;
  } catch (const decode_exception &) {
    return false;
  }
}

}  // namespace json
}  // namespace spotify
