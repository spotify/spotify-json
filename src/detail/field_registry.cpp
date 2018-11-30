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

#include <spotify/json/detail/field_registry.hpp>

#include <spotify/json/codec/string.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace detail {

namespace {
std::string escape_key(const std::string &key) {
  encode_context context;
  codec::string().encode(context, key);
  context.append(':');
  return std::string(context.data(), context.size());
}
} // anonymous namespace

field_registry::field_registry() = default;
field_registry::~field_registry() = default;
field_registry::field_registry(const field_registry &) = default;
field_registry::field_registry(field_registry &&) = default;

void field_registry::save_field(const std::string &name, bool required,
                                const std::shared_ptr<void> &f) {
  const auto was_saved =
      _fields.insert(typename field_map::value_type(name, f)).second;
  if (was_saved) {
    _field_list.push_back(std::make_pair(escape_key(name), f));
    _num_required_fields += size_t(required);
  }
}

const void *field_registry::find_field(const std::string &name) const noexcept {
  const auto field_it = _fields.find(name);
  if (json_unlikely(field_it == _fields.end())) {
    return nullptr;
  }
  return (*field_it).second.get();
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
