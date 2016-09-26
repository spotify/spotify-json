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

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

// See https://functionalcpp.wordpress.com/2013/08/05/function-traits/

template <class F>
struct function_traits;
 
// function pointer
template <class R, class... args_type>
struct function_traits<R(*)(args_type...)> : public function_traits<R(args_type...)> {
};
 
template <class R, class... args_type>
struct function_traits<R(args_type...)> {
  using return_type = R;

  static constexpr std::size_t arity = sizeof...(args_type);

  template <std::size_t N>
  struct argument {
    static_assert(N < arity, "error: invalid parameter index.");
    using type = typename std::tuple_element<N,std::tuple<args_type...>>::type;
  };
};

// member function pointer
template <class C, class R, class... args_type>
struct function_traits<R(C::*)(args_type...)> : public function_traits<R(C&, args_type...)> {
};
 
// const member function pointer
template <class C, class R, class... args_type>
struct function_traits<R(C::*)(args_type...) const> : public function_traits<R(C&, args_type...)> {
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
    typename codec_type,
    typename encode_transform,
    typename decode_transform>
class transform_t final {
 public:
  using object_type = typename std::result_of<decode_transform(typename codec_type::object_type, size_t)>::type;

  transform_t(
      codec_type inner_codec,
      encode_transform encode,
      decode_transform decode)
      : _inner_codec(std::move(inner_codec)),
        _encode_transform(std::move(encode)),
        _decode_transform(std::move(decode)) {}

  object_type decode(decode_context &context) const {
    const auto offset = context.offset();  // Capture offset before decoding
    return _decode_transform(_inner_codec.decode(context), offset);
  }

  void encode(encode_context &context, const object_type &value) const {
    _inner_codec.encode(context, _encode_transform(value));
  }

 private:
  codec_type _inner_codec;
  encode_transform _encode_transform;
  decode_transform _decode_transform;
};

template <
    typename codec_type,
    typename encode_transform,
    typename decode_transform,
    typename transform_codec_type = transform_t<
        typename std::decay<codec_type>::type,
        typename std::decay<encode_transform>::type,
        typename std::decay<decode_transform>::type>>
transform_codec_type transform(codec_type &&inner_codec, encode_transform &&encode, decode_transform &&decode) {
  return transform_codec_type(
      std::forward<codec_type>(inner_codec),
      std::forward<encode_transform>(encode),
      std::forward<decode_transform>(decode));
}

/**
 * Variant of transform that uses the default codec for the type that the
 * encode_transform function returns. This only works if encode_transform is a
 * function-like thing that can only return one type.
 */
template <
    typename encode_transform,
    typename decode_transform,
    typename inner_type = typename std::decay<typename detail::function_traits<encode_transform>::return_type>::type,
    typename codec_type = decltype(default_codec<inner_type>()),
    typename transform_codec_type = transform_t<
        codec_type,
        typename std::decay<encode_transform>::type,
        typename std::decay<decode_transform>::type>>
transform_codec_type transform(encode_transform &&encode, decode_transform &&decode) {
  return transform_codec_type(
      default_codec<inner_type>(),
      std::forward<encode_transform>(encode),
      std::forward<decode_transform>(decode));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
