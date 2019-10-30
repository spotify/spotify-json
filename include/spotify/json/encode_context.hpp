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
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

/**
 * An encode_context has the information that is kept while encoding JSON with
 * codecs. It keeps a buffer of data that can be expanded and written to.
 */
struct encode_context final {
  encode_context(const std::size_t capacity = 4096);
  ~encode_context();

  json_force_inline char *reserve(const std::size_t reserved_bytes) {
    const auto remaining_bytes = static_cast<std::size_t>(_end - _ptr);  // _end is always >= _ptr
    if (json_likely(remaining_bytes >= reserved_bytes)) {
      return _ptr;
    } else {
      return grow_buffer(reserved_bytes);
    }
  }

  json_force_inline void advance(const std::size_t num_bytes) {
    _ptr += num_bytes;
  }

  json_force_inline void append(const char c) {
    reserve(1)[0] = c;
    advance(1);
  }

  json_force_inline void append_or_replace(const char replacing, const char with) {
    if (json_likely(!empty() && _ptr[-1] == replacing)) {
      _ptr[-1] = with;
    } else {
      append(with);
    }
  }

  json_force_inline void append(const void *data, const std::size_t size) {
    std::memcpy(reserve(size), data, size);
    advance(size);
  }

  json_never_inline void clear() {
    _ptr = _buf;
  }

  json_force_inline const char *data() const {
    return _buf;
  }

  json_force_inline std::size_t size() const {
    return static_cast<std::size_t>(_ptr - _buf);
  }

  json_force_inline std::size_t capacity() const {
    return _capacity;
  }

  json_force_inline bool empty() const {
    return (_ptr == _buf);
  }

  std::unique_ptr<void, decltype(std::free) *> steal_data();

  const bool has_sse42;

 private:
  char * grow_buffer(const std::size_t num_bytes);

  char *_buf;
  char *_ptr;
  const char *_end;
  std::size_t _capacity;
};

}  // namespace json
}  // namespace spotify
