/*
 * Copyright (c) 2016-2019 Spotify AB
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

#include <cstddef>
#include <memory>
#include <vector>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

struct bitset_base {
  ~bitset_base();

  json_force_inline uint8_t test_and_set(const std::size_t index) {
    const auto byte = (index / 8);
    const auto bidx = (index & 7);
    const auto mask = (1 << bidx);
    const auto byte_before = _base[byte];
    _base[byte] = (byte_before | mask);
    return (byte_before & mask) >> bidx;
  }

 protected:
  bitset_base(const std::size_t size, uint8_t *inline_base);

 private:
  std::unique_ptr<std::vector<uint8_t>> _vector;
  uint8_t *_base;
};

template <std::size_t inline_size>
struct bitset final : public bitset_base {
  bitset(const std::size_t size)
      : bitset_base(size, size <= inline_size ? _array : nullptr) {
    if (json_likely(size <= inline_size)) {
      memset(_array, 0, sizeof(_array));
    }
  }

 private:
  static constexpr auto _num_inline_bytes = ((inline_size + 7) / 8);
  uint8_t _array[_num_inline_bytes];
};

}  // namespace detail
}  // namespace json
}  // namespace spotify
