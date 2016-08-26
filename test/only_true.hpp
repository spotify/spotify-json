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

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/boolean.hpp>
#include <spotify/json/decoding_context.hpp>

namespace spotify {
namespace json {
namespace codec {

/**
 * Codec for booleans that only encodes true.
 */
class only_true_t final {
 public:
  using object_type = bool;

  object_type decode(decoding_context &context) const {
    return object_type();
  }

  void encode(encoding_context &context, const object_type &value) const {
    _bool_codec.encode(context, true);
  }

  bool should_encode(const object_type &value) const {
    return value;
  }

 private:
  boolean_t _bool_codec;
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
