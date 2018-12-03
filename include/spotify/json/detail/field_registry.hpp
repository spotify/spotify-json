/*
 * Copyright (c) 2018 Spotify AB
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

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace spotify {
namespace json {
namespace detail {

// Non-templated class to reduce code bloat.
class field_registry final {
 public:
  using field_vec = std::vector<std::pair<std::string, std::shared_ptr<const void>>>;
  using field_map = std::unordered_map<std::string, std::shared_ptr<const void>>;
  using const_iterator = typename field_vec::const_iterator;

  field_registry();
  ~field_registry();
  field_registry(const field_registry &);
  field_registry(field_registry &&);

  // Forward the iterator implementation so range based for works.
  inline const_iterator begin() const noexcept { return _field_list.begin(); }
  inline const_iterator end() const noexcept { return _field_list.end(); }

  void save(const std::string &name, bool required, const std::shared_ptr<void> &f);
  const void *find(const std::string &name) const noexcept;
  size_t num_required_fields() const noexcept { return _num_required_fields; }

 private:
  field_vec _field_list;
  field_map _fields;
  size_t _num_required_fields = 0;
};

}  // namespace detail
}  // namespace json
}  // namespace spotify
