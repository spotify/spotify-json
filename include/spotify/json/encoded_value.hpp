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

#include <cstdlib>
#include <string>
#include <ostream>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/skip_value.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {

struct ref {
  using value_type = char;

  ref(const char *data, std::size_t size)
      : _data(data),
        _size(size) {}

  ref(const char *begin, const char *end)
      : _data(begin),
        _size(end - begin) {}

  const char *data() const { return _data; }
  std::size_t size() const { return _size; }

 private:
  const char *_data;
  std::size_t _size;
};

template <typename storage_type = std::string>
struct encoded_value {
  struct unsafe_unchecked {};

  explicit encoded_value(storage_type json)
      : _json(std::move(json)) {
    decode_context context(_json.data(), _json.size());
    detail::skip_value(context);  // validate provided JSON string
    detail::fail_if(context, context.position != context.end, "Unexpected trailing input");
  }

  encoded_value() : encoded_value("null", 4, unsafe_unchecked()) {}

  encoded_value(encode_context &&context, const unsafe_unchecked &) : encoded_value(
      context.data(),
      context.size(),
      unsafe_unchecked()) {}

  encoded_value(const char *data, std::size_t size, const unsafe_unchecked &)
      : _json(data, data + size) {}

  operator storage_type const &() const & { return _json; }
  operator storage_type &&() && { return std::move(_json); }

  const char *data() const { return _json.data(); }
  std::size_t size() const { return _json.size(); }

 private:
  storage_type _json;
};

template <typename storage_type>
bool operator ==(const encoded_value<storage_type> &lhs, const storage_type &rhs) {
  return static_cast<const storage_type &>(lhs) == rhs;
}

template <typename storage_type>
bool operator ==(const storage_type &lhs, const encoded_value<storage_type> &rhs) {
  return lhs == static_cast<const storage_type &>(rhs);
}

inline bool operator ==(const encoded_value<std::string> &lhs, const char *const rhs) {
  return static_cast<const std::string &>(lhs) == rhs;
}

inline bool operator ==(const char *const lhs, const encoded_value<std::string> &rhs) {
  return lhs == static_cast<const std::string &>(rhs);
}

template <typename storage_type>
bool operator !=(const encoded_value<storage_type> &lhs, const storage_type &rhs) {
  return static_cast<const storage_type &>(lhs) != rhs;
}

template <typename storage_type>
bool operator !=(const storage_type &lhs, const encoded_value<storage_type> &rhs) {
  return lhs != static_cast<const storage_type &>(rhs);
}

inline bool operator !=(const encoded_value<std::string> &lhs, const char *const rhs) {
  return static_cast<const std::string &>(lhs) != rhs;
}

inline bool operator !=(const char *const lhs, const encoded_value<std::string> &rhs) {
  return lhs != static_cast<const std::string &>(rhs);
}

template <typename storage_type>
std::ostream &operator <<(std::ostream &stream, const encoded_value<storage_type> &value) {
  return stream << static_cast<const storage_type &>(value);
}

}  // namespace json
}  // namespace spotify
