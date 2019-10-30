/*
 * Copyright (c) 2015-2019 Spotify AB
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

#include <spotify/json/codec/object.hpp>

namespace spotify {
namespace json {
namespace codec {
namespace codec_detail {

object_t_base::object_t_base() = default;
object_t_base::object_t_base(construct_untyped *construct) : _construct(construct) {}
object_t_base::object_t_base(object_t_base &&) = default;
object_t_base::object_t_base(const object_t_base &) = default;
object_t_base::~object_t_base() = default;

void object_t_base::decode(decode_context &context, void *value) const {
  uint_fast32_t uniq_seen_required = 0;
  detail::bitset<64> seen_required(_fields.num_required_fields());

  detail::decode_object<string_t>(context, [&](const std::string &key) {
    const auto *field = _fields.find(key);
    if (json_unlikely(!field)) {
      return detail::skip_value(context);
    }

    field->decode(context, value);
    if (field->is_required()) {
      const auto seen = seen_required.test_and_set(field->required_field_idx());
      uniq_seen_required += (1 - seen);  // 'seen' is 1 when the field is a duplicate; 0 otherwise
    }
  });

  const auto is_missing_req_fields = (uniq_seen_required != _fields.num_required_fields());
  detail::fail_if(context, is_missing_req_fields, "Missing required field(s)");
}

void object_t_base::encode(encode_context &context, const void *value) const {
  context.append('{');
  for (const auto &kv : _fields) {
    const auto &field = *kv.second.get();
    field.encode(context, kv.first, value);
  }
  context.append_or_replace(',', '}');
}

}  // namespace codec_detail
}  // namespace codec
}  // namespace json
}  // namespace spotify
