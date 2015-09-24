/*
 * Copyright (c) 2015 Spotify AB
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

#include <spotify/json/buffer.hpp>
#include <spotify/json/default_codec.hpp>

namespace spotify {
namespace json {
namespace detail {

inline void requireAtEnd(const decoding_context &context) {
  if (context.position != context.end) {
    throw decode_exception("Unexpected trailing input", context.offset());
  }
}

}

template<typename Codec>
void encode(const Codec &codec, const typename Codec::object_type &object, buffer &buffer) {
  writer w(buffer);
  codec.encode(object, w);
}

template<typename Codec>
std::string encode(const Codec &codec, const typename Codec::object_type &object) {
  buffer buffer;
  writer w(buffer);
  codec.encode(object, w);
  return std::string(buffer.data(), buffer.size());
}


template<typename Value>
std::string encode(const Value &value) {
  buffer buffer;
  writer w(buffer);
  default_codec<Value>().encode(value, w);
  return std::string(buffer.data(), buffer.size());
}

template<typename Codec>
typename Codec::object_type decode(const Codec &codec, const char *data, size_t size) {
  decoding_context c(data, data + size);
  const auto result = codec.decode(c);
  detail::requireAtEnd(c);
  return result;
}

template<typename Codec>
typename Codec::object_type decode(const Codec &codec, const std::string &string) {
  return decode(codec, string.data(), string.size());
}

template<typename Codec>
typename Codec::object_type decode(const Codec &codec, const buffer &buffer) {
  return decode(codec, buffer.data(), buffer.size());
}

template<typename Value>
Value decode(const std::string &string) {
  return decode(default_codec<Value>(), string);
}

template<typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const char *data,
    size_t size) {
  decoding_context c(data, data + size);
  try {
    object = codec.decode(c);
    detail::requireAtEnd(c);
    return true;
  } catch (decode_exception &) {
    return false;
  }
}

template<typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const std::string &string) {
  return try_decode(object, codec, string.data(), string.size());
}

template<typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const buffer &buffer) {
  return try_decode(object, codec, buffer.data(), buffer.size());
}

template<typename Value>
bool try_decode(Value &object, const std::string &string) {
  return try_decode(object, default_codec<Value>(), string);
}

}  // namespace json
}  // namespace spotify
