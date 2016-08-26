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

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

/**
 * An encoding_context has the information that is kept while encoding JSON with
 * codecs. It keeps a buffer of data that can be expanded and written to.
 */
struct encoding_context final {
  encoding_context(const size_t capacity = 4096)
      : _buf(static_cast<uint8_t *>(std::malloc(capacity))),
        _ptr(_buf),
        _end(_buf + capacity),
        _capacity(capacity) {
    if (!_buf && _capacity > 0) {
      throw std::bad_alloc();
    }
  }

  ~encoding_context() {
    std::free(_buf);
  }

  json_force_inline uint8_t *reserve(const size_t num_bytes) {
    if (json_unlikely(_ptr + num_bytes >= _end)) {
      grow_buffer(num_bytes);
    }
    return _ptr;
  }

  json_force_inline void advance(const size_t num_bytes) {
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

  json_force_inline void append(const void *data, const size_t size) {
    std::memcpy(reserve(size), data, size);
    advance(size);
  }

  json_force_inline const uint8_t *data() const {
    return _buf;
  }

  json_force_inline size_t size() const {
    return (_ptr - _buf);
  }

  json_force_inline size_t capacity() const {
    return _capacity;
  }

  json_force_inline bool empty() const {
    return (_ptr == _buf);
  }

 private:
  json_never_inline void grow_buffer(const size_t num_bytes) {
    const auto old_size = size();
    const auto new_size = (old_size + num_bytes);
    const auto new_capacity = std::max(new_size, _capacity * 2);

    if (!(_buf = static_cast<uint8_t *>(std::realloc(_buf, new_capacity)))) {
      throw std::bad_alloc();
    }

    _ptr = _buf + old_size;
    _end = _buf + new_capacity;
    _capacity = new_capacity;
   }

  uint8_t *_buf;
  uint8_t *_ptr;
  const uint8_t *_end;
  size_t _capacity;
};

}  // namespace json
}  // namespace spotify
