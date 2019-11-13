/*
 * Copyright (c) 2014-2019 Spotify AB
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
#include <spotify/json/codec/optional.hpp>
#include <spotify/json/codec/smart_ptr.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T>
struct make_smart_ptr_t<boost::shared_ptr<T>> {
  template <typename O>
  static boost::shared_ptr<T> make(O &&object) {
    using object_type = typename std::decay<O>::type;
    return boost::make_shared<object_type>(std::forward<O>(object));
  }
};

template <typename codec_type>
using boost_shared_ptr_t = detail::smart_ptr_t<codec_type, boost::shared_ptr<typename codec_type::object_type>>;

template <typename codec_type>
boost_shared_ptr_t<typename std::decay<codec_type>::type> boost_shared_ptr(codec_type &&inner_codec) {
  return boost_shared_ptr_t<typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
}

template <typename T, typename F>
struct codec_cast<boost::shared_ptr<T>, boost::shared_ptr<F>> {
  static boost::shared_ptr<T> cast(const boost::shared_ptr<F> &ptr) {
    return boost::dynamic_pointer_cast<T>(ptr);
  }
};

template <typename codec_type>
using boost_optional_t = optional_t<codec_type, boost::optional<typename codec_type::object_type>, boost::none_t>;

template <typename codec_type>
boost_optional_t<typename std::decay<codec_type>::type> boost_optional(codec_type &&inner_codec) {
  return boost_optional_t<typename std::decay<codec_type>::type>(std::forward<codec_type>(inner_codec));
}

}  // namespace codec

template <typename T>
struct default_codec_t<boost::shared_ptr<T>> {
  static decltype(codec::boost_shared_ptr(default_codec<T>())) codec() {
    return codec::boost_shared_ptr(default_codec<T>());
  }
};

template <typename T>
struct default_codec_t<boost::optional<T>> {
  static decltype(codec::boost_optional(default_codec<T>())) codec() {
    return codec::boost_optional(default_codec<T>());
  }
};

template <typename rep_type, typename period_type>
struct default_codec_t<boost::chrono::duration<rep_type, period_type>> {
  static decltype(codec::duration<boost::chrono::duration<rep_type, period_type>>()) codec() {
    return codec::duration<boost::chrono::duration<rep_type, period_type>>();
  }
};

template <typename clock_type, typename duration_type>
struct default_codec_t<boost::chrono::time_point<clock_type, duration_type>> {
  static decltype(codec::time_point<boost::chrono::time_point<clock_type, duration_type>>()) codec() {
    return codec::time_point<boost::chrono::time_point<clock_type, duration_type>>();
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
