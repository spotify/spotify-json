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

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace detail {

inline void require_at_end(const decoding_context &context) {
  if (context.position != context.end) {
    detail::fail(context, "Unexpected trailing input");
  }
}

}  // namespace detail

template <typename Codec>
json_never_inline std::string encode(const Codec &codec, const typename Codec::object_type &object) {
  encoding_context context;
  codec.encode(context, object);
  return std::string(reinterpret_cast<const char *>(context.data()), context.size());
}

template <typename Value>
json_never_inline std::string encode(const Value &value) {
  return encode(default_codec<Value>(), value);
}

template <typename Codec>
typename Codec::object_type decode(const Codec &codec, const char *data, size_t size) {
  decoding_context c(data, data + size);
  detail::advance_past_whitespace(c);
  const auto result = codec.decode(c);
  detail::advance_past_whitespace(c);
  detail::require_at_end(c);
  return result;
}

template <typename Codec>
typename Codec::object_type decode(const Codec &codec, const std::string &string) {
  return decode(codec, string.data(), string.size());
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
  decoding_context c(data, data + size);
  try {
    detail::advance_past_whitespace(c);
    object = codec.decode(c);
    detail::advance_past_whitespace(c);
    detail::require_at_end(c);
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

template <typename Codec>
bool try_decode_partial(
    typename Codec::object_type &object,
    const Codec &codec,
    const decoding_context &context) {
  decoding_context c(context);
  try {
    detail::advance_past_whitespace(c);
    object = codec.decode(c);
    return true;
  } catch (const decode_exception &) {
    return false;
  }
}

}  // namespace json
}  // namespace spotify
