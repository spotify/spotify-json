/*
 * Copyright (c) 2014 Spotify AB
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

#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <spotify/json/codec/cast.hpp>
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/detail/pair.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {

template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const boost::optional<T> &optional) {
  if (optional) {
    writer << optional.get();
  }
  return writer;
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const pair<K, boost::optional<V> > &pair) {
  if (pair.value) {
    writer.add_pair(pair.key, pair.value.get());
  }
  return writer;
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::pair<K, boost::optional<V> > &pair) {
  if (pair.second) {
    writer.add_pair(pair.first, pair.second.get());
  }
  return writer;
}

/// boost::shared_ptr

namespace codec {

template<typename T>
struct make_smart_ptr_t<boost::shared_ptr<T>> {
  static boost::shared_ptr<T> make(T &&obj) {
    return boost::make_shared<T>(std::forward<T>(obj));
  }
};

template<typename InnerCodec>
using boost_shared_ptr_t = detail::smart_ptr_t<InnerCodec, boost::shared_ptr<typename InnerCodec::object_type>>;

template<typename InnerCodec>
boost_shared_ptr_t<InnerCodec> boost_shared_ptr(InnerCodec &&inner_codec) {
  return boost_shared_ptr_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

template<typename ToType, typename FromType>
struct codec_cast<boost::shared_ptr<ToType>, boost::shared_ptr<FromType>> {
  static boost::shared_ptr<ToType> cast(const boost::shared_ptr<FromType> &ptr) {
    return boost::dynamic_pointer_cast<ToType>(ptr);
  }
};

}  // namespace codec

template<typename T>
struct default_codec_t<boost::shared_ptr<T>> {
  static decltype(boost_shared_ptr(default_codec<T>())) codec() {
    return boost_shared_ptr(default_codec<T>());
  }
};

/// boost::optional

namespace codec {

template<typename InnerCodec>
class optional_t final {
 public:
  using object_type = boost::optional<typename InnerCodec::object_type>;

  explicit optional_t(InnerCodec inner_codec)
      : _inner_codec(inner_codec) {}

  void encode(const object_type &value, writer &w) const {
    if (value) {
      _inner_codec.encode(*value, w);
    }
  }

  object_type decode(decoding_context &context) const {
    return _inner_codec.decode(context);
  }

  bool should_encode(const object_type &value) const {
    return value != boost::none;
  }

 private:
  InnerCodec _inner_codec;
};

template<typename InnerCodec>
optional_t<InnerCodec> optional(InnerCodec &&inner_codec) {
  return optional_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

}  // namespace codec

template<typename T>
struct default_codec_t<boost::optional<T>> {
  static decltype(codec::optional(default_codec<T>())) codec() {
    return codec::optional(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
