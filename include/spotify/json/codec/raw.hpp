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

#include <cstdlib>
#include <cstring>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/skip_value.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

struct raw_ref {
  raw_ref() : _data(nullptr), _size(0) {}
  raw_ref(const char *data, std::size_t size) : _data(data), _size(size) {}
  raw_ref(const char *begin, const char *end) : _data(begin), _size(end - begin) {}

  explicit operator decode_context() const { return decode_context(data(), size()); }

  const char *data() const { return _data; }
  std::size_t size() const { return _size; }

 private:
  const char *_data;
  std::size_t _size;
};

template <typename T>
class raw_t final {
 public:
  using object_type = T;

  object_type decode(decode_context &context) const {
    const auto begin = context.position;
    detail::skip_value(context);
    return object_type(begin, static_cast<std::size_t>(context.position - begin));
  }

  void encode(encode_context &context, const object_type &value) const {
    std::memcpy(context.reserve(value.size()), value.data(), value.size());
    context.advance(value.size());
  }
};

template <typename T>
inline raw_t<T> raw() {
  return raw_t<T>();
}

}  // namespace codec

template <>
struct default_codec_t<codec::raw_ref> {
  static codec::raw_t<codec::raw_ref> codec() {
    return codec::raw<codec::raw_ref>();
  }
};

}  // namespace json
}  // namespace spotify
