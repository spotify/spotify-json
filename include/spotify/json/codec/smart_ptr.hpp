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

#include <utility>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/writer.hpp>

namespace spotify {
namespace json {
namespace detail {

template<typename SmartPointer>
struct make_smart_ptr_t;

template<typename T>
struct make_smart_ptr_t<std::unique_ptr<T>> {
  static std::unique_ptr<T> make(T &&obj) {
    return std::unique_ptr<T>(new T(std::forward<T>(obj)));
  }
};

template<typename T>
struct make_smart_ptr_t<std::shared_ptr<T>> {
  static std::shared_ptr<T> make(T &&obj) {
    return std::make_shared<T>(std::forward<T>(obj));
  }
};

template<typename InnerCodec, typename SmartPointer>
class smart_ptr_t {
 public:
  using object_type = SmartPointer;

  explicit smart_ptr_t(InnerCodec inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  void encode(const object_type &value, writer &writer) const {
    _inner_codec.encode(*value, writer);
  }

  object_type decode(decoding_context &context) const {
    return make_smart_ptr_t<object_type>::make(_inner_codec.decode(context));
  }

 protected:
  InnerCodec _inner_codec;
};

}  // namespace detail

namespace codec {

template<typename InnerCodec>
using unique_ptr_t = detail::smart_ptr_t<InnerCodec, std::unique_ptr<typename InnerCodec::object_type>>;

template<typename InnerCodec>
using shared_ptr_t = detail::smart_ptr_t<InnerCodec, std::shared_ptr<typename InnerCodec::object_type>>;

template<typename InnerCodec>
unique_ptr_t<InnerCodec> unique_ptr(InnerCodec &&inner_codec) {
  return unique_ptr_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

template<typename InnerCodec>
shared_ptr_t<InnerCodec> shared_ptr(InnerCodec &&inner_codec) {
  return shared_ptr_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec

template<typename T>
struct default_codec_t<std::unique_ptr<T>> {
  static decltype(unique_ptr(default_codec<T>())) codec() {
    return unique_ptr(default_codec<T>());
  }
};

template<typename T>
struct default_codec_t<std::shared_ptr<T>> {
  static decltype(shared_ptr(default_codec<T>())) codec() {
    return shared_ptr(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
