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

#include <iterator>  // std::iterator, std::input_iterator_tag

namespace spotify {
namespace json {
namespace detail {

template<typename T>
class iterator : public std::iterator<std::input_iterator_tag, T> {
 public:
  explicit iterator(T *ptr = nullptr) : _ptr(ptr) {}

  T &operator* () { return *_ptr; }

  iterator &operator++ () { ++_ptr; return *this; }
  iterator operator++ (int) { iterator it = *this; ++_ptr; return it; }

  bool operator== (const iterator &other) const { return _ptr == other._ptr; }
  bool operator!= (const iterator &other) const { return _ptr != other._ptr; }

 private:
  T *_ptr;
};

}  // namespace detail
}  // namespace json
}  // namespace spotify
