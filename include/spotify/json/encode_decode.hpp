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

template<typename Codec>
void encode(const Codec &codec, const typename Codec::object_type &object, buffer &buffer) {
  detail::writer w(buffer);
  codec.encode(object, w);
}

template<typename Codec>
std::string encode(const Codec &codec, const typename Codec::object_type &object) {
  buffer buffer;
  detail::writer w(buffer);
  codec.encode(object, w);
  return std::string(buffer.data(), buffer.size());
}


template<typename Value>
std::string encode(const Value &value) {
  buffer buffer;
  detail::writer writer(buffer);
  default_codec<Value>().encode(value, writer);
  return std::string(buffer.data(), buffer.size());
}

template<typename Codec>
typename Codec::object_type decode(const Codec &codec, const char *data, size_t size) {
  decoding_context c(data, data + size);
  auto obj = codec.decode(c);
  // FIXME(peck) Handle errors and when the parse didn't consume everything
  return obj;  // FIXME(peck) move??
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

}  // namespace json
}  // namespace spotify
