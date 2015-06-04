/*
 * Copyright (c) 2014 Spotify AB
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

#include <string>

#include <spotify/json/detail/escape.hpp>

namespace spotify {
namespace json {
namespace detail {

class key {
  template<typename InputIterator, typename InputEndIterator>
  static std::string escape_for_storage(const InputIterator &begin, const InputEndIterator &end) {
    std::string storage("\"");
    detail::write_escaped(storage, begin, end);
    storage.append("\"");
    return storage;
  }

 public:
  explicit key(const char *raw)
      : _storage(escape_for_storage(raw, detail::null_terminated_end_iterator())),
        data(_storage.data()),
        size(_storage.size()) {}

  key(const char *raw, int length)
      : _storage(escape_for_storage(raw, raw + length)),
        data(_storage.data()),
        size(_storage.size()) {}

  template<typename Iterable>
  explicit key(const Iterable &iterable)
      : _storage(escape_for_storage(iterable.begin(), iterable.end())),
        data(_storage.data()),
        size(_storage.size()) {}

  key(const key &other)
      : _storage(other._storage),
        data(_storage.data()),
        size(_storage.size()) {}

  key(key &&other)
      : _storage(std::move(other._storage)),
        data(_storage.data()),
        size(_storage.size()) {}

  key &operator=(const key &other) = delete;

 private:
  std::string _storage;

 public:
  const char * const data;
  const size_t size;
};

}  // namespace detail
}  // namespace json
}  // namespace spotify
