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

#include <spotify/json/encode_context.hpp>

#include <algorithm>
#include <limits>
#include <spotify/json/detail/cpuid.hpp>

namespace spotify {
namespace json {

encode_context::encode_context(const std::size_t capacity)
    : has_sse42(detail::cpuid().has_sse42()),
      _buf(static_cast<char *>(capacity ? std::malloc(capacity) : nullptr)),
      _ptr(_buf),
      _end(_buf + capacity),
      _capacity(capacity) {
  if (json_unlikely(!_buf && _capacity > 0)) {
    throw std::bad_alloc();
  }
}

encode_context::~encode_context() {
  std::free(_buf);
}

std::unique_ptr<void, decltype(std::free) *> encode_context::steal_data() {
  const auto data = _buf;
  _buf = nullptr;
  _ptr = nullptr;
  _end = nullptr;
  _capacity = 0;
  return std::unique_ptr<void, decltype(std::free) *>(data, &std::free);
}

char *encode_context::grow_buffer(const std::size_t num_bytes) {
  const auto old_size = size();
  const auto new_size = std::size_t(old_size + num_bytes);
  if (json_unlikely(new_size < old_size)) {
    // If we overflow the size integer, it means that we need more memory than
    // we can possibly provide, so we should throw an allocation exception.
    throw std::bad_alloc();
  }

  auto new_capacity = std::size_t(_capacity * 2);
  if (json_unlikely(new_capacity <= _capacity && _capacity)) {
    // If we overflow the capacity integer, set the new capacity to the max
    // value of the size type, so that we can handle the case of having say
    // 3 GB of memory allocated, growing to 4 GB instead of failing to grow.
    new_capacity = std::numeric_limits<std::size_t>::max();
  }

  // Regardless of what capacity we think we want, we need to ensure that it
  // is at least as large as the reserved size. We avoid doing any arithmetics
  // here to not have to check for overflow yet again.
  const auto actual_capacity = std::max(new_size, new_capacity);
  _buf = static_cast<char *>(std::realloc(_buf, actual_capacity));
  if (json_unlikely(!_buf)) {
    throw std::bad_alloc();
  }

  _ptr = _buf + old_size;
  _end = _buf + actual_capacity;
  _capacity = actual_capacity;

  return _ptr;
}

}  // namespace json
}  // namespace spotify
