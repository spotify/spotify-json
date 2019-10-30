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
#include <cstring>
#include <memory>
#include <new>
#include <ostream>

#include <spotify/json/decode_context.hpp>
#include <spotify/json/detail/decode_helpers.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/detail/skip_value.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

struct encoded_value_base {
  struct unsafe_unchecked {};

 protected:
  void validate_json(const char *data, std::size_t size);
};

}  // namespace detail

struct encoded_value;

struct encoded_value_ref : public detail::encoded_value_base {
  encoded_value_ref();
  encoded_value_ref(const encoded_value &value);
  explicit encoded_value_ref(const char *cstr);
  explicit encoded_value_ref(const char *cstr, const unsafe_unchecked &);
  explicit encoded_value_ref(const char *data, std::size_t size);
  explicit encoded_value_ref(const char *data, std::size_t size, const unsafe_unchecked &);

  template <typename value_with_data_and_size>
  explicit encoded_value_ref(const value_with_data_and_size &json);

  const char *data() const { return _data; }
  std::size_t size() const { return _size; }

  void swap(encoded_value_ref &value_ref);

 private:
  std::size_t _size;
  const char *_data;
};

struct encoded_value : public detail::encoded_value_base {
  encoded_value();
  encoded_value(encoded_value &&value) noexcept;
  encoded_value(const encoded_value &value);
  encoded_value(const encoded_value_ref &value_ref);
  ~encoded_value();

  explicit encoded_value(const char *cstr);
  explicit encoded_value(const char *cstr, const unsafe_unchecked &);
  explicit encoded_value(const char *data, std::size_t size);
  explicit encoded_value(const char *data, std::size_t size, const unsafe_unchecked &);
  explicit encoded_value(encode_context &&context);
  explicit encoded_value(encode_context &&context, const unsafe_unchecked &);

  template <typename value_with_data_and_size>
  explicit encoded_value(const value_with_data_and_size &json);

  encoded_value &operator=(encoded_value &&value) noexcept;
  encoded_value &operator=(const encoded_value &value);
  encoded_value &operator=(const encoded_value_ref &value_ref);

  const char *data() const { return static_cast<const char *>(_data.get()); }
  std::size_t size() const { return _size; }

  void swap(encoded_value &value);

 private:
  using data_buffer = std::unique_ptr<void, decltype(std::free) *>;
  std::size_t _size;
  data_buffer _data;
};

inline encoded_value_ref::encoded_value_ref(const encoded_value &value)
    : encoded_value_ref(value.data(), value.size(), unsafe_unchecked()) {}

inline encoded_value_ref::encoded_value_ref(const char *cstr)
    : encoded_value_ref(cstr, std::strlen(cstr)) {}

inline encoded_value_ref::encoded_value_ref(const char *cstr, const unsafe_unchecked &)
    : encoded_value_ref(cstr, std::strlen(cstr), unsafe_unchecked()) {}

inline encoded_value_ref::encoded_value_ref(const char *data, std::size_t size, const unsafe_unchecked &)
    : _size(size),
      _data(data) {}

template <typename value_with_data_and_size>
encoded_value_ref::encoded_value_ref(const value_with_data_and_size &json)
    : encoded_value_ref(json.data(), json.size()) {}

inline encoded_value::encoded_value(encoded_value &&value) noexcept
    : encoded_value() {
  swap(value);
}

inline encoded_value::encoded_value(const encoded_value &value)
    : encoded_value(value.data(), value.size(), unsafe_unchecked()) {}

inline encoded_value::encoded_value(const encoded_value_ref &value_ref)
    : encoded_value(value_ref.data(), value_ref.size(), unsafe_unchecked()) {}

inline encoded_value::encoded_value(const char *cstr)
    : encoded_value(cstr, std::strlen(cstr)) {}

inline encoded_value::encoded_value(const char *cstr, const unsafe_unchecked &)
    : encoded_value(cstr, std::strlen(cstr), unsafe_unchecked()) {}

inline encoded_value::encoded_value(encode_context &&context, const unsafe_unchecked &)
    : _size(context.size()),
      _data(context.steal_data()) {}

template <typename value_with_data_and_size>
encoded_value::encoded_value(const value_with_data_and_size &json)
    : encoded_value(json.data(), json.size()) {}

std::ostream &operator <<(std::ostream &stream, const encoded_value_ref &value);
std::ostream &operator <<(std::ostream &stream, const encoded_value &value);

bool operator==(const encoded_value_ref &a, const encoded_value_ref &b);
bool operator!=(const encoded_value_ref &a, const encoded_value_ref &b);

}  // namespace json
}  // namespace spotify
