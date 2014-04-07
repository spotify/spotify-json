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

#include "detail/json_escape.hpp"

namespace json {

class key {
 public:
  explicit key(const char *raw)
      : _storage("\""),
        _ref(detail::write_escaped(_storage, raw, detail::null_terminated_end_iterator())),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

  key(const char *raw, int length)
      : _storage("\""),
        _ref(detail::write_escaped(_storage, raw, raw + length)),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

  template<typename Iterable>
  explicit key(const Iterable &iterable)
      : _storage("\""),
        _ref(detail::write_escaped(_storage, iterable.begin(), iterable.end())),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

 private:
  std::string _storage;
  std::string &_ref;

 public:
  const char *data;
  const size_t size;
};

}  // namespace json
