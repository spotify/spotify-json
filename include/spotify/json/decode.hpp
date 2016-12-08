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

#include <cstring>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

/*
 * json::decode(codec, data...)
 */

template <typename codec_type>
typename codec_type::object_type decode(const codec_type &codec, const char *data, size_t size) {
  decode_context c(data, data + size);
  detail::skip_any_whitespace(c);
  const auto result = codec.decode(c);
  detail::skip_any_whitespace(c);
  detail::fail_if(c, c.position != c.end, "Unexpected trailing input");
  return result;
}

template <typename codec_type>
typename codec_type::object_type decode(const codec_type &codec, const char *cstr) {
  return decode(codec, cstr, std::strlen(cstr));
}

template <typename codec_type, typename string_type>
typename codec_type::object_type decode(const codec_type &codec, const string_type &string) {
  return decode(codec, string.data(), string.size());
}

/*
 * json::decode(data...)
 */

template <typename value_type>
value_type decode(const char *data, size_t size) {
  return decode(default_codec<value_type>(), data, size);
}

template <typename value_type>
value_type decode(const char *cstr) {
  return decode(default_codec<value_type>(), cstr, std::strlen(cstr));
}

template <typename value_type, typename string_type>
value_type decode(const string_type &string) {
  return decode(default_codec<value_type>(), string);
}

/*
 * json::try_decode(&object, codec, data...)
 */

template <typename codec_type>
bool try_decode(
    typename codec_type::object_type &object,
    const codec_type &codec,
    const char *data,
    size_t size) noexcept {
  try {
    object = decode(codec, data, size);
    return true;
  } catch (...) {
    return false;
  }
}

template <typename codec_type>
bool try_decode(
    typename codec_type::object_type &object,
    const codec_type &codec,
    const char *cstr) noexcept {
  return try_decode(object, codec, cstr, std::strlen(cstr));
}

template <typename codec_type, typename string_type>
bool try_decode(
    typename codec_type::object_type &object,
    const codec_type &codec,
    const string_type &string) noexcept {
  return try_decode(object, codec, string.data(), string.size());
}

/*
 * json::try_decode(&object, data...)
 */

template <typename value_type>
bool try_decode(value_type &object, const char *data, size_t size) noexcept {
  return try_decode(object, default_codec<value_type>(), data, size);
}

template <typename value_type>
bool try_decode(value_type &object, const char *cstr) noexcept {
  return try_decode(object, default_codec<value_type>(), cstr, std::strlen(cstr));
}

template <typename value_type, typename string_type>
bool try_decode(value_type &object, const string_type &string) noexcept {
  return try_decode(object, default_codec<value_type>(), string);
}

}  // namespace json
}  // namespace spotify
