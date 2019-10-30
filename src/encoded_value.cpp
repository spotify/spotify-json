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

#include <spotify/json/encoded_value.hpp>

#include <algorithm>
#include <limits>
#include <spotify/json/detail/cpuid.hpp>

namespace spotify {
namespace json {
namespace detail {

void encoded_value_base::validate_json(const char *data, std::size_t size) {
  decode_context context(data, size);
  detail::skip_value(context);  // validate provided JSON string
  detail::fail_if(context, context.position != context.end, "Unexpected trailing input");
}

}  // namespace detail

encoded_value_ref::encoded_value_ref()
    : _size(4),
      _data("null") {}

encoded_value_ref::encoded_value_ref(const char *data, std::size_t size)
    : encoded_value_ref(data, size, unsafe_unchecked()) {
  validate_json(encoded_value_ref::data(), encoded_value_ref::size());
}

void encoded_value_ref::swap(encoded_value_ref &value_ref) {
  std::swap(_size, value_ref._size);
  std::swap(_data, value_ref._data);
}

encoded_value::encoded_value()
    : _size(4),
      _data((void *) "null", [](void *) noexcept {}) {}

encoded_value::~encoded_value() = default;

encoded_value::encoded_value(const char *data, std::size_t size)
    : encoded_value(data, size, unsafe_unchecked()) {
  validate_json(encoded_value::data(), encoded_value::size());
}

encoded_value::encoded_value(encode_context &&context)
    : encoded_value(std::move(context), unsafe_unchecked()) {
  validate_json(data(), size());
}

encoded_value::encoded_value(const char *data, std::size_t size, const unsafe_unchecked &)
    : _size(size),
      _data(std::malloc(size), &std::free) {
  if (json_unlikely(!_data && size)) {
    throw std::bad_alloc();
  }
  std::memcpy(_data.get(), data, _size);
}

encoded_value &encoded_value::operator=(encoded_value &&value) noexcept {
  swap(value);
  return *this;
}

encoded_value &encoded_value::operator=(const encoded_value &value) {
  encoded_value new_value(value);
  swap(new_value);
  return *this;
}

encoded_value &encoded_value::operator=(const encoded_value_ref &value_ref) {
  encoded_value new_value(value_ref);
  swap(new_value);
  return *this;
}

void encoded_value::swap(encoded_value &value) {
  std::swap(_size, value._size);
  std::swap(_data, value._data);
}

std::ostream &operator <<(std::ostream &stream, const encoded_value_ref &value) {
  stream.write(value.data(), value.size());
  return stream;
}

std::ostream &operator <<(std::ostream &stream, const encoded_value &value) {
  stream.write(value.data(), value.size());
  return stream;
}

bool operator==(const encoded_value_ref &a, const encoded_value_ref &b) {
  return
      a.size() == b.size() &&
      (a.data() == b.data() || std::memcmp(a.data(), b.data(), a.size()) == 0);
}

bool operator!=(const encoded_value_ref &a, const encoded_value_ref &b) {
  return !(a == b);
}

}  // namespace json
}  // namespace spotify
