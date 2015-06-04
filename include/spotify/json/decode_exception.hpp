/*
 * Copyright (c) 2015 Spotify AB
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

#include <stdexcept>
#include <string>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

class decode_exception : public std::runtime_error {
 public:
  template <typename string_type>
  json_never_inline decode_exception(const string_type &what, const size_t offset)
      : runtime_error(what),
        _what(what),
        _offset(offset) {}

  virtual const char *what() const throw() {
    return _what.c_str();
  }

  size_t offset() const {
    return _offset;
  }

 private:
  const std::string _what;
  const off_t _offset;
};

}  // namespace json
}  // namespace spotify
