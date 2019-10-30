/*
 * Copyright (c) 2015-2016 Spotify AB
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
#include <utility>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {

/**
 * decode_exception objects are thrown when decoding fails, for example if the
 * JSON is invalid, or if the JSON doesn't conform to the specified schema.
 */
class decode_exception final : public std::runtime_error {
 public:
  explicit decode_exception(const char *what, size_t offset = 0);
  decode_exception(decode_exception &&exception, size_t offset);

  size_t offset() const {
    return _offset;
  }

 private:
  size_t _offset;
};

}  // namespace json
}  // namespace spotify
