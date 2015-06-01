/*
 * Copyright (c) 2014-2015 Spotify AB
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

#include <spotify/json/detail/macros.hpp>
#include <spotify/json/error.hpp>

namespace spotify {
namespace json {

/**
 * A decoding_context has the information that is kept while decoding JSON with
 * codecs. It has information about the data to read and whether the decoding
 * has failed.
 */
struct decoding_context final {
  decoding_context(const char *begin, const char *end)
      : position(begin),
        begin(begin),
        end(end) {}

  bool has_failed() const {
    return !error.empty();
  }

  template <typename string_type>
  void require_bytes(const size_t needed, const string_type &error) const {
    if (position + needed > end) {
      throw decode_exception(error, offset());
    }
  }

  template <typename string_type>
  void require_bytes(const string_type &error) const {
    if (position == end) {
      throw decode_exception(error, offset());
    }
  }

  template <typename string_type>
  void require(const bool condition, const string_type &error) const {
    if (!condition) {
      throw decode_exception(error, offset());
    }
  }

  template <typename string_type>
  json_noreturn void fail(const string_type &error, const ssize_t d = 0) const {
    throw decode_exception(error, offset(d));
  }

  off_t offset(const ssize_t d = 0) const {
    return (position - begin) + d;
  }

  /**
   * A non-empty error indicates that the parsing failed.
   */
  std::string error;

  /**
   * Pointer to the current position of the decoding process. If error is
   * non-empty, position points to the position of the error.
   *
   * position must never point beyond end.
   */
  const char *position;
  const char * const begin;
  const char * const end;
};

}  // namespace json
}  // namespace spotify
