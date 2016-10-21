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

#include <memory>
#include <utility>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T>
class any_codec_t final {
 public:
  using object_type = T;

  template <typename codec_type>
  explicit any_codec_t(codec_type codec)
      : _codec(std::make_shared<erased_codec_impl<codec_type>>(std::move(codec))) {}

  object_type decode(decode_context &context) const {
    return _codec->decode(context);
  }

  void encode(encode_context &context, const object_type &value) const {
    _codec->encode(context, value);
  }

  bool should_encode(const object_type &value) const {
    return _codec->should_encode(value);
  }

 private:
  class erased_codec {
   public:
    virtual ~erased_codec() = default;

    virtual object_type decode(decode_context &context) const = 0;
    virtual void encode(encode_context &context, const object_type &value) const = 0;
    virtual bool should_encode(const object_type &value) const = 0;
  };

  template <typename codec_type>
  class erased_codec_impl final : public erased_codec {
   public:
    explicit erased_codec_impl(codec_type codec)
      : _codec(std::move(codec)) {}

    object_type decode(decode_context &context) const override {
      return _codec.decode(context);
    }

    void encode(encode_context &context, const object_type &value) const override {
      _codec.encode(context, value);
    }

    bool should_encode(const object_type &value) const override {
      return detail::should_encode(_codec, value);
    }

   private:
    const codec_type _codec;
  };

  std::shared_ptr<const erased_codec> _codec;
};

template <typename codec_type>
any_codec_t<typename codec_type::object_type> any_codec(codec_type &&codec) {
  return any_codec_t<typename codec_type::object_type>(std::forward<codec_type>(codec));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
