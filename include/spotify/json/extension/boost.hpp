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

}  // namespace codec

template<typename T>
struct default_codec_t<boost::shared_ptr<T>> {
  static decltype(boost_shared_ptr(default_codec<T>())) codec() {
    return boost_shared_ptr(default_codec<T>());
  }
};

}  // namespace json
}  // namespace spotify
