/*
 * Copyright (c) 2014-2016 Spotify AB
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

#include <boost/chrono.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <spotify/json/codec/cast.hpp>
#include <spotify/json/codec/chrono.hpp>
#include <spotify/json/codec/map.hpp>
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T>
struct make_smart_ptr_t<boost::shared_ptr<T>> {
  template <typename Obj>
  static boost::shared_ptr<T> make(Obj &&obj) {
    using ObjectType = typename std::decay<Obj>::type;
    return boost::make_shared<ObjectType>(std::forward<Obj>(obj));
  }
};

template <typename InnerCodec>
using boost_shared_ptr_t = detail::smart_ptr_t<InnerCodec, boost::shared_ptr<typename InnerCodec::object_type>>;

template <typename InnerCodec>
boost_shared_ptr_t<typename std::decay<InnerCodec>::type> boost_shared_ptr(
    InnerCodec &&inner_codec) {
  return boost_shared_ptr_t<typename std::decay<InnerCodec>::type>(
      std::forward<InnerCodec>(inner_codec));
}

template <typename ToType, typename FromType>
struct codec_cast<boost::shared_ptr<ToType>, boost::shared_ptr<FromType>> {
  static boost::shared_ptr<ToType> cast(const boost::shared_ptr<FromType> &ptr) {
    return boost::dynamic_pointer_cast<ToType>(ptr);
  }
};

template <typename InnerCodec>
class optional_t final {
 public:
  using object_type = boost::optional<typename InnerCodec::object_type>;

  explicit optional_t(InnerCodec inner_codec)
      : _inner_codec(inner_codec) {}

  object_type decode(decode_context &context) const {
    return _inner_codec.decode(context);
  }

  void encode(encode_context &context, const object_type &value) const {
    detail::fail_if(context, !value, "Cannot encode uninitialized optional");
    _inner_codec.encode(context, *value);
  }

  bool should_encode(const object_type &value) const {
    return (value != boost::none) && detail::should_encode(_inner_codec, *value);
  }

 private:
  InnerCodec _inner_codec;
};

template <typename InnerCodec, typename... Options>
optional_t<typename std::decay<InnerCodec>::type> optional(InnerCodec &&inner_codec,
                                                           Options... options) {
  return optional_t<typename std::decay<InnerCodec>::type>(std::forward<InnerCodec>(inner_codec),
                                                           options...);
}

}  // namespace codec

template <typename T>
struct default_codec_t<boost::shared_ptr<T>> {
  static decltype(boost_shared_ptr(default_codec<T>())) codec() {
    return boost_shared_ptr(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<boost::optional<T>> {
  static decltype(codec::optional(default_codec<T>())) codec() {
    return codec::optional(default_codec<T>());
  }
};

template <typename Rep, typename Period>
struct default_codec_t<boost::chrono::duration<Rep, Period>> {
  static decltype(codec::duration<boost::chrono::duration<Rep, Period>>()) codec() {
    return codec::duration<boost::chrono::duration<Rep, Period>>();
  }
};

template <typename Clock, typename Duration>
struct default_codec_t<boost::chrono::time_point<Clock, Duration>> {
  static decltype(codec::time_point<boost::chrono::time_point<Clock, Duration>>()) codec() {
    return codec::time_point<boost::chrono::time_point<Clock, Duration>>();
  }
};

template <typename T>
struct default_codec_t<boost::container::flat_map<std::string, T>> {
  static decltype(codec::map<boost::container::flat_map<std::string, T>>(default_codec<T>())) codec() {
    return codec::map<boost::container::flat_map<std::string, T>>(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
