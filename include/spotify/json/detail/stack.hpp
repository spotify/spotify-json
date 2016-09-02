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

#include <array>
#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>
#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

template <typename T, std::size_t inline_capacity>
struct stack {
  void push(T value) {
    if (json_unlikely(_vector)) {
      _vector->push_back(std::move(value));
    } else if (json_likely(_inline_size < inline_capacity)) {
      _array[_inline_size++] = std::move(value);
    } else {
      _vector.reset(new std::vector<T>(_array.begin(), _array.end()));
      _vector->push_back(std::move(value));
    }
  }

  T pop() {
    if (json_unlikely(_vector)) {
      auto top = _vector->back();
      _vector->pop_back();
      return top;
    } else {
      assert(_inline_size);
      return _array[--_inline_size];
    }
  }

 private:
  std::array<T, inline_capacity> _array;
  std::unique_ptr<std::vector<T>> _vector;
  std::size_t _inline_size;
};

}  // namespace detail
}  // namespace json
}  // namespace spotify
