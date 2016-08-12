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

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace detail {

// See https://functionalcpp.wordpress.com/2013/08/05/function-traits/

template <class F>
struct function_traits;
 
// function pointer
template <class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)> {
};
 
template <class R, class... Args>
struct function_traits<R(Args...)> {
  using return_type = R;

  static constexpr std::size_t arity = sizeof...(Args);

  template <std::size_t N>
  struct argument {
    static_assert(N < arity, "error: invalid parameter index.");
    using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
  };
};

// member function pointer
template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)> {
};
 
// const member function pointer
template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)> {
};
 
// member object pointer
template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)> {
};

// functor
template <class F>
struct function_traits {
 private:
  using call_type = function_traits<decltype(&F::type::operator())>;
 public:
  using return_type = typename call_type::return_type;

  static constexpr std::size_t arity = call_type::arity - 1;

  template <std::size_t N>
  struct argument {
    static_assert(N < arity, "error: invalid parameter index.");
    using type = typename call_type::template argument<N+1>::type;
  };
};
 
template <class F>
struct function_traits<F&> : public function_traits<F> {
};
 
template <class F>
struct function_traits<F&&> : public function_traits<F> {
};

}  // namespace detail

namespace codec {

/**
 * There are types that spotify-json doesn't have built-in support for that
 * aren't structs (this case is handled by object_t). For these types,
 * transform_t can be used. transform_t codecs are created with a pair of
 * conversion functions: One that transforms an object that is about to be
 * encoded into a type that another encoder can handle, and another that
 * transforms an object that has just been decoded back to the outwards facing
 * type.
 *
 * This can be used for example when encoding a SHA1 hash which is encoded in
 * JSON as a string.
 *
 * The provided transform function for decoding can throw a decode_exception, if
 * the JSON value can't be parsed. The decode transform function is provided
 * with two parameters: The value to decode and the offset of decoding, to be
 * passed to decode_exception if something goes wrong.
 */
template <
    typename InnerCodec,
    typename EncodeTransform,
    typename DecodeTransform>
class transform_t final {
 public:
  using object_type = typename std::result_of<
      DecodeTransform (typename InnerCodec::object_type, size_t)>::type;

  transform_t(
      InnerCodec inner_codec,
      EncodeTransform encode_transform,
      DecodeTransform decode_transform)
      : _inner_codec(std::move(inner_codec)),
        _encode_transform(std::move(encode_transform)),
        _decode_transform(std::move(decode_transform)) {}

  void encode(const object_type &value, detail::writer &w) const {
    _inner_codec.encode(_encode_transform(value), w);
  }

  object_type decode(decoding_context &context) const {
    const auto offset = context.offset();  // Capture offset before decoding
    return _decode_transform(_inner_codec.decode(context), offset);
  }

 private:
  InnerCodec _inner_codec;
  EncodeTransform _encode_transform;
  DecodeTransform _decode_transform;
};

template <typename InnerCodec,
          typename EncodeTransform,
          typename DecodeTransform,
          typename Transform = transform_t<typename std::decay<InnerCodec>::type,
                                           typename std::decay<EncodeTransform>::type,
                                           typename std::decay<DecodeTransform>::type>>
Transform transform(InnerCodec &&inner_codec,
                    EncodeTransform &&encode_transform,
                    DecodeTransform &&decode_transform) {
  return Transform(std::forward<InnerCodec>(inner_codec),
                   std::forward<EncodeTransform>(encode_transform),
                   std::forward<DecodeTransform>(decode_transform));
}

/**
 * Variant of transform that uses the default codec for the type that the
 * EncodeTransform function returns. This only works if EncodeTransform is a
 * function-like thing that can only return one type.
 */
template <typename EncodeTransform,
          typename DecodeTransform,
          typename InnerType = typename std::decay<
              typename detail::function_traits<EncodeTransform>::return_type>::type,
          typename InnerCodec = decltype(default_codec<InnerType>()),
          typename Transform = transform_t<InnerCodec,
                                           typename std::decay<EncodeTransform>::type,
                                           typename std::decay<DecodeTransform>::type>>
Transform transform(EncodeTransform &&encode_transform, DecodeTransform &&decode_transform) {
  return Transform(default_codec<InnerType>(),
                   std::forward<EncodeTransform>(encode_transform),
                   std::forward<DecodeTransform>(decode_transform));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
