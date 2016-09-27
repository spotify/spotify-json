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

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/encode_helpers.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

/**
 * Codec that maps values from a set of JSON values to values of another C++
 * type. This is useful for enums.
 */
template <typename outer_type, typename codec_type>
class enumeration_t final {
  using inner_type = typename codec_type::object_type;
  using mapping_type = std::vector<std::pair<outer_type, inner_type>>;

 public:
  using object_type = outer_type;

  enumeration_t(codec_type inner_codec, mapping_type &&mapping)
      : _inner_codec(std::move(inner_codec)),
        _mapping(std::move(mapping)) {}

  object_type decode(decode_context &context) const {
    const auto result = _inner_codec.decode(context);
    const auto it = std::find_if(_mapping.begin(), _mapping.end(), [&](const std::pair<outer_type, inner_type> &pair) {
      return pair.second == result;
    });
    detail::fail_if(context, it == _mapping.end(), "Encountered unknown enumeration value");
    return it->first;
  }

  void encode(encode_context &context, const object_type &value) const {
    const auto it = find(value);
    detail::fail_if(context, it == _mapping.end(), "Encoding unknown enumeration value");
    _inner_codec.encode(context, (*it).second);
  }

  bool should_encode(const object_type &value) const {
    return find(value) != _mapping.end();
  }

 private:
  json_never_inline typename mapping_type::const_iterator find(const object_type &value) const {
    return std::find_if(_mapping.begin(), _mapping.end(), [&](const std::pair<outer_type, inner_type> &pair) {
      return pair.first == value;
    });
  }

  codec_type _inner_codec;
  mapping_type _mapping;
};

template <typename outer_type, typename codec_type>
enumeration_t<outer_type, typename std::decay<codec_type>::type> enumeration(
    codec_type &&inner_codec,
    std::initializer_list<std::pair<outer_type, typename codec_type::object_type>> objs) {
  std::vector<std::pair<outer_type, typename codec_type::object_type>> mapping;
  for (const auto &obj : objs) {
    mapping.push_back(obj);
  }
  return enumeration_t<outer_type, typename std::decay<codec_type>::type>(
      std::forward<codec_type>(inner_codec), std::move(mapping));
}

template <typename outer_type, typename InnerObject>
enumeration_t<outer_type, decltype(default_codec<InnerObject>())> enumeration(
    std::initializer_list<std::pair<outer_type, InnerObject>> objs) {
  std::vector<std::pair<outer_type, InnerObject>> mapping;
  for (const auto &obj : objs) {
    mapping.push_back(obj);
  }
  return enumeration_t<outer_type, decltype(default_codec<InnerObject>())>(
      default_codec<InnerObject>(),
      std::move(mapping));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
