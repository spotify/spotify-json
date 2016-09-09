/*
 * Copyright (c) 2016 Spotify AB
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

#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encode_exception.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename string_type>
json_never_inline json_noreturn void fail(
    const encoding_context &context,
    const string_type &error) {
  throw encode_exception(error);
}

template <typename string_type, typename condition_type>
json_force_inline void fail_if(
    const encoding_context &context,
    const condition_type condition,
    const string_type &error) {
  if (json_unlikely(condition)) {
    fail(context, error);
  }
}

template <typename T>
struct has_should_encode_method {
  template <typename U>
  static auto test(int) -> decltype(
      std::declval<U>().should_encode(std::declval<typename U::object_type>()),
      std::true_type());

  template <typename>
  static std::false_type test(...);

 public:
  static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};

template <typename Codec>
typename std::enable_if<!has_should_encode_method<Codec>::value, bool>::type
json_force_inline should_encode(const Codec &codec, const typename Codec::object_type &value) {
  return true;
}

template <typename Codec>
typename std::enable_if<has_should_encode_method<Codec>::value, bool>::type
json_force_inline should_encode(const Codec &codec, const typename Codec::object_type &value) {
  return codec.should_encode(value);
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
