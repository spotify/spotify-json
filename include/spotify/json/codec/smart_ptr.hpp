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

#include <utility>

#include <spotify/json/codec/standard.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

namespace detail {

template<
    typename InnerCodec,
    template<class> class SmartPointer,
    SmartPointer<typename InnerCodec::object_type> MakePointer(
        typename InnerCodec::object_type &&)>
class smart_ptr_t {
 public:
  using object_type = SmartPointer<typename InnerCodec::object_type>;

  smart_ptr_t(InnerCodec inner_codec)
      : _inner_codec(std::move(inner_codec)) {}

  void encode(const object_type &value, writer &writer) const {
    _inner_codec.encode(*value, writer);
  }

  object_type decode(decoding_context &context) const {
    return MakePointer(_inner_codec.decode(context));
  }

 protected:
  InnerCodec _inner_codec;
};

// This template is here so that there is a unique_ptr type that, like std::shared_ptr, takes only
// one template parameter. Otherwise it can't be passed in as the SmartPointer template parameter
// to smart_ptr_t.
template<typename T>
using unique_ptr = std::unique_ptr<T>;

template<typename T>
std::unique_ptr<T> make_unique(T &&obj) {
  return std::unique_ptr<T>(new T(std::forward<T>(obj)));
}

template<typename T>
std::shared_ptr<T> make_shared(T &&obj) {
  return std::make_shared<T>(std::forward<T>(obj));
}

}  // namespace detail

template<typename InnerCodec>
using unique_ptr_t = detail::smart_ptr_t<InnerCodec, detail::unique_ptr, detail::make_unique>;

template<typename InnerCodec>
using shared_ptr_t = detail::smart_ptr_t<InnerCodec, std::shared_ptr, detail::make_shared>;

template<typename InnerCodec>
unique_ptr_t<InnerCodec> unique_ptr(InnerCodec &&inner_codec) {
  return unique_ptr_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

template<typename InnerCodec>
shared_ptr_t<InnerCodec> shared_ptr(InnerCodec &&inner_codec) {
  return shared_ptr_t<InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

template<typename T>
struct standard_t<std::unique_ptr<T>> {
  static decltype(unique_ptr(standard<T>())) codec() {
    return unique_ptr(standard<T>());
  }
};

template<typename T>
struct standard_t<std::shared_ptr<T>> {
  static decltype(shared_ptr(standard<T>())) codec() {
    return shared_ptr(standard<T>());
  }
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
