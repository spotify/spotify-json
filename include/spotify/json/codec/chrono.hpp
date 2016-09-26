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

#include <chrono>

#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/transform.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename duration_type>
typename duration_type::rep encodeTransformDuration(duration_type duration) {
  return duration.count();
}

template <typename duration_type>
duration_type decodeTransformDuration(typename duration_type::rep duration_rep, size_t offset) {
  return duration_type(duration_rep);
}

template <typename time_point_type>
typename time_point_type::rep encodeTransformTimePoint(time_point_type time_point) {
  return time_point.time_since_epoch().count();
}

template <typename time_point_type>
time_point_type decodeTransformTimePoint(typename time_point_type::rep duration_rep, size_t offset) {
  return time_point_type(typename time_point_type::duration(duration_rep));
}

}  // namespace detail
namespace codec {

template <typename duration_type>
decltype(transform(
    &detail::encodeTransformDuration<duration_type>,
    &detail::decodeTransformDuration<duration_type>))
duration() {
  return transform(
      &detail::encodeTransformDuration<duration_type>,
      &detail::decodeTransformDuration<duration_type>);
}

template <typename time_point_type>
decltype(transform(
    &detail::encodeTransformTimePoint<time_point_type>,
    &detail::decodeTransformTimePoint<time_point_type>))
time_point() {
  return transform(
      &detail::encodeTransformTimePoint<time_point_type>,
      &detail::decodeTransformTimePoint<time_point_type>);
}

}  // namespace codec

template <typename Rep, typename Period>
struct default_codec_t<std::chrono::duration<Rep, Period>> {
  static decltype(codec::duration<std::chrono::duration<Rep, Period>>()) codec() {
    return codec::duration<std::chrono::duration<Rep, Period>>();
  }
};

template <typename Clock, typename duration_type>
struct default_codec_t<std::chrono::time_point<Clock, duration_type>> {
  static decltype(codec::time_point<std::chrono::time_point<Clock, duration_type>>()) codec() {
    return codec::time_point<std::chrono::time_point<Clock, duration_type>>();
  }
};

}  // namespace json
}  // namespace spotify
