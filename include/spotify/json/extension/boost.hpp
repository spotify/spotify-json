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

#include <boost/chrono.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <spotify/json/codec/cast.hpp>
#include <spotify/json/codec/chrono.hpp>
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
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

  explicit optional_t(InnerCodec inner_codec, bool none_as_null = false)
      : _inner_codec(inner_codec), _none_as_null(none_as_null) {}

  void encode(const object_type &value, writer &w) const {
    if (value) {
      _inner_codec.encode(*value, w);
    } else {
      w.add_null();
    }
  }

  object_type decode(decoding_context &context) const {
    if (_none_as_null) {
      detail::require_bytes<1>(context);
      if (detail::peek_unchecked(context) == 'n') {
        detail::advance_past_null(context);
        return boost::none;
      }
    }

    return _inner_codec.decode(context);
  }

  bool should_encode(const object_type &value) const {
    return _none_as_null || (value != boost::none);
  }

 private:
  InnerCodec _inner_codec;
  bool _none_as_null;
};

template<typename InnerCodec>
optional_t<InnerCodec> optional(InnerCodec &&inner_codec, bool none_as_null = false) {
  return optional_t<InnerCodec>(std::forward<InnerCodec>(inner_codec), none_as_null);
}

}  // namespace codec

template<typename T>
struct default_codec_t<boost::optional<T>> {
  static decltype(codec::optional(default_codec<T>())) codec() {
    return codec::optional(default_codec<T>());
  }
};

/// boost::chrono types

template<typename Rep, typename Period>
struct default_codec_t<boost::chrono::duration<Rep, Period>> {
  static decltype(codec::duration<boost::chrono::duration<Rep, Period>>()) codec() {
    return codec::duration<boost::chrono::duration<Rep, Period>>();
  }
};

template<typename Clock, typename Duration>
struct default_codec_t<boost::chrono::time_point<Clock, Duration>> {
  static decltype(codec::time_point<boost::chrono::time_point<Clock, Duration>>()) codec() {
    return codec::time_point<boost::chrono::time_point<Clock, Duration>>();
  }
};

namespace boost_detail {

template<typename WriterType, typename Iterable>
inline WriterType &write_object(WriterType &writer, const Iterable &iterable) {
  const typename WriterType::scoped_object object(writer);
  for (typename Iterable::const_iterator it = iterable.begin(); it != iterable.end(); ++it) {
    writer << *it;
  }
  return writer;
}

}  // namespace boost_detail

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const boost::container::flat_map<K, V> &map) {
  return boost_detail::write_object(writer, map);
}

}  // namespace json
}  // namespace spotify
