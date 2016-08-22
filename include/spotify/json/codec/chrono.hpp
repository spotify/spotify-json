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

#include <chrono>

#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/transform.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename Duration>
typename Duration::rep encodeTransformDuration(Duration duration) {
  return duration.count();
}

template <typename Duration>
Duration decodeTransformDuration(
    typename Duration::rep duration_rep, size_t offset) {
  return Duration(duration_rep);
}

template <typename TimePoint>
typename TimePoint::rep encodeTransformTimePoint(TimePoint time_point) {
  return time_point.time_since_epoch().count();
}

template <typename TimePoint>
TimePoint decodeTransformTimePoint(
    typename TimePoint::rep duration_rep, size_t offset) {
  return TimePoint(typename TimePoint::duration(duration_rep));
}

}  // namespace detail
namespace codec {

template <typename Duration>
decltype(transform(
    &detail::encodeTransformDuration<Duration>,
    &detail::decodeTransformDuration<Duration>))
duration() {
  return transform(
      &detail::encodeTransformDuration<Duration>,
      &detail::decodeTransformDuration<Duration>);
}

template <typename TimePoint>
decltype(transform(
    &detail::encodeTransformTimePoint<TimePoint>,
    &detail::decodeTransformTimePoint<TimePoint>))
time_point() {
  return transform(
      &detail::encodeTransformTimePoint<TimePoint>,
      &detail::decodeTransformTimePoint<TimePoint>);
}

}  // namespace codec

template <typename Rep, typename Period>
struct default_codec_t<std::chrono::duration<Rep, Period>> {
  static decltype(codec::duration<std::chrono::duration<Rep, Period>>()) codec() {
    return codec::duration<std::chrono::duration<Rep, Period>>();
  }
};

template <typename Clock, typename Duration>
struct default_codec_t<std::chrono::time_point<Clock, Duration>> {
  static decltype(codec::time_point<std::chrono::time_point<Clock, Duration>>()) codec() {
    return codec::time_point<std::chrono::time_point<Clock, Duration>>();
  }
};

}  // namespace json
}  // namespace spotify
