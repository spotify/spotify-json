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

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

template <std::size_t inline_size>
struct bitset {
  bitset(const std::size_t size) {
    if (json_likely(size <= inline_size)) {
      _array.fill(0x00);
    } else {
      _vector.reset(new std::vector<uint8_t>((size + 7) / 8));
    }
  }

  json_force_inline uint8_t test_and_set(const std::size_t index) {
    const auto byte = (index / 8);
    const auto bidx = (index & 7);
    const auto mask = (1 << bidx);
    if (json_likely(!_vector)) {
      const auto byte_before = _array[byte];
      _array[byte] = (byte_before | mask);
      return (byte_before & mask) >> bidx;
    } else {
      const auto byte_before = (*_vector)[byte];
      (*_vector)[byte] = (byte_before | mask);
      return (byte_before & mask) >> bidx;
    }
  }

 private:
  static constexpr auto _num_inline_bytes = ((inline_size + 7) / 8);
  std::unique_ptr<std::vector<uint8_t>> _vector;
  std::array<uint8_t, _num_inline_bytes> _array;
};

}  // namespace detail
}  // namespace json
}  // namespace spotify
