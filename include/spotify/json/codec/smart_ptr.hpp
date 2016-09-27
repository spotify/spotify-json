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

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/encode_helpers.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T>
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

template <typename codec_type, typename T>
class smart_ptr_t final {
 public:
  using object_type = T;

  explicit smart_ptr_t(codec_type inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  object_type decode(decode_context &context) const {
    return codec::make_smart_ptr_t<object_type>::make(_inner_codec.decode(context));
  }

  void encode(encode_context &context, const object_type &value) const {
    detail::fail_if(context, !value, "Cannot encode null smart pointer");
    _inner_codec.encode(context, *value);
  }

  bool should_encode(const object_type &value) const {
    return bool(value);
  }

 protected:
  codec_type _inner_codec;
};

}  // namespace detail

namespace codec {

template <typename codec_type>
using unique_ptr_t = detail::smart_ptr_t<codec_type, std::unique_ptr<typename codec_type::object_type>>;

template <typename codec_type>
using shared_ptr_t = detail::smart_ptr_t<codec_type, std::shared_ptr<typename codec_type::object_type>>;

template <typename codec_type>
unique_ptr_t<typename std::decay<codec_type>::type> unique_ptr(codec_type &&inner_codec) {
  return unique_ptr_t<typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
}

template <typename codec_type>
shared_ptr_t<typename std::decay<codec_type>::type> shared_ptr(codec_type &&inner_codec) {
  return shared_ptr_t<typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
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
