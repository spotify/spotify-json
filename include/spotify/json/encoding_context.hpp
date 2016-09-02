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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

/**
 * An encoding_context has the information that is kept while encoding JSON with
 * codecs. It keeps a buffer of data that can be expanded and written to.
 */
template <typename size_type = std::size_t>
struct base_encoding_context final {
  base_encoding_context(const size_type capacity = 4096)
      : _buf(static_cast<uint8_t *>(capacity ? std::malloc(capacity) : nullptr)),
        _ptr(_buf),
        _end(_buf + capacity),
        _capacity(capacity) {
    if (json_unlikely(!_buf && _capacity > 0)) {
      throw std::bad_alloc();
    }
  }

  ~base_encoding_context() {
    std::free(_buf);
  }

  json_force_inline uint8_t *reserve(const size_type num_bytes) {
    const auto remaining_bytes = (_end - _ptr);  // _end is always >= _ptr
    if (json_unlikely(remaining_bytes < num_bytes)) {
      grow_buffer(num_bytes);
    }
    return _ptr;
  }

  json_force_inline void advance(const size_type num_bytes) {
    _ptr += num_bytes;
  }

  json_force_inline void append(const uint8_t c) {
    reserve(1)[0] = c;
    advance(1);
  }

  json_force_inline void append_or_replace(
      const uint8_t replacing,
      const uint8_t with) {
    if (json_unlikely(empty() || _ptr[-1] != replacing)) {
      append(with);
    } else {
      _ptr[-1] = with;
    }
  }

  json_force_inline void append(const void *data, const size_type size) {
    std::memcpy(reserve(size), data, size);
    advance(size);
  }

  json_force_inline const uint8_t *data() const {
    return _buf;
  }

  json_force_inline size_type size() const {
    return (_ptr - _buf);
  }

  json_force_inline size_type capacity() const {
    return _capacity;
  }

  json_force_inline bool empty() const {
    return (_ptr == _buf);
  }

 private:
  json_never_inline void grow_buffer(const size_type num_bytes) {
    const auto old_size = size();
    const auto new_size = size_type(old_size + num_bytes);
    if (json_unlikely(new_size <= old_size)) {
      // If we overflow the size integer, it means that we need more memory than
      // we can provide, so we should throw an allocation exception. Note that
      // we check for <= instead of < to catch the case of adding SIZE_MAX to
      // SIZE_MAX, which equals SIZE_MAX. We already know that num_bytes will
      // not be equal to zero (no need to grow the buffer in that case).
      throw std::bad_alloc();
    }

    auto new_capacity = size_type(_capacity * 2);
    if (json_unlikely(new_capacity <= _capacity && _capacity)) {
      // If we overflow the capacity integer, set the new capacity to the max
      // value of the size type, so that we can handle the case of having say
      // 3 GB of memory allocated, growing to 4 GB instead of failing to grow.
      new_capacity = std::numeric_limits<size_type>::max();
    }

    // Regardless of what capacity we think we want, we need to ensure that it
    // is at least as large as the reserved size. We avoid doing any arithmetics
    // here to not have to check for overflow yet again.
    const auto actual_capacity = std::max(new_size, new_capacity);
    _buf = static_cast<uint8_t *>(std::realloc(_buf, actual_capacity));
    if (json_unlikely(!_buf)) {
      throw std::bad_alloc();
    }

    _ptr = _buf + old_size;
    _end = _buf + actual_capacity;
    _capacity = actual_capacity;
  }

  uint8_t *_buf;
  uint8_t *_ptr;
  const uint8_t *_end;
  size_type _capacity;
};

}  // namespace detail

using encoding_context = detail::base_encoding_context<>;

}  // namespace json
}  // namespace spotify
