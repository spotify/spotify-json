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

#include <utility>

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/writer.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename SmartPointer>
struct make_smart_ptr_t;

template <typename T>
struct make_smart_ptr_t<std::unique_ptr<T>> {
  template <typename Obj>
  static std::unique_ptr<T> make(Obj &&obj) {
    using ObjectType = typename std::decay<Obj>::type;
    return std::unique_ptr<T>(new ObjectType(std::forward<Obj>(obj)));
  }
};

template <typename T>
struct make_smart_ptr_t<std::shared_ptr<T>> {
  template <typename Obj>
  static std::shared_ptr<T> make(Obj &&obj) {
    using ObjectType = typename std::decay<Obj>::type;
    return std::make_shared<ObjectType>(std::forward<Obj>(obj));
  }
};

}  // namespace codec

namespace detail {

template <typename InnerCodec, typename SmartPointer>
class smart_ptr_t {
 public:
  using object_type = SmartPointer;

  explicit smart_ptr_t(InnerCodec inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  object_type decode(decoding_context &context) const {
    return codec::make_smart_ptr_t<object_type>::make(_inner_codec.decode(context));
  }

  void encode(const object_type &value, detail::writer &writer) const {
    if (json_unlikely(!value)) {
      throw std::logic_error("Cannot encode null smart pointer");
    }
    _inner_codec.encode(*value, writer);
  }

  void encode(encoding_context &context, const object_type &value) const {
    if (json_unlikely(!value)) {
      throw std::logic_error("Cannot encode null smart pointer");
    }
    _inner_codec.encode(context, *value);
  }

  bool should_encode(const object_type &value) const { return bool(value); }

 protected:
  InnerCodec _inner_codec;
};

}  // namespace detail

namespace codec {

template <typename InnerCodec>
using unique_ptr_t = detail::smart_ptr_t<InnerCodec, std::unique_ptr<typename InnerCodec::object_type>>;

template <typename InnerCodec>
using shared_ptr_t = detail::smart_ptr_t<InnerCodec, std::shared_ptr<typename InnerCodec::object_type>>;

template <typename InnerCodec>
unique_ptr_t<typename std::decay<InnerCodec>::type> unique_ptr(InnerCodec &&inner_codec) {
  return unique_ptr_t<typename std::decay<InnerCodec>::type>(std::forward<InnerCodec>(inner_codec));
}

template <typename InnerCodec>
shared_ptr_t<typename std::decay<InnerCodec>::type> shared_ptr(InnerCodec &&inner_codec) {
  return shared_ptr_t<typename std::decay<InnerCodec>::type>(std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec

template <typename T>
struct default_codec_t<std::unique_ptr<T>> {
  static decltype(codec::unique_ptr(default_codec<T>())) codec() {
    return codec::unique_ptr(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<std::shared_ptr<T>> {
  static decltype(codec::shared_ptr(default_codec<T>())) codec() {
    return codec::shared_ptr(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
