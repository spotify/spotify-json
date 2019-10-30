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

#include <spotify/json/detail/bitset.hpp>

namespace spotify {
namespace json {
namespace detail {

bitset_base::~bitset_base() = default;

bitset_base::bitset_base(const std::size_t size, uint8_t *inline_base) {
  if (inline_base) {
    _base = inline_base;
  } else {
    _vector.reset(new std::vector<uint8_t>((size + 7) / 8));
    _base = _vector->data();
  }
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
