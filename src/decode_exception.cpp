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

#include <spotify/json/decode_exception.hpp>

namespace spotify {
namespace json {

decode_exception::decode_exception(const char *what, size_t offset)
    : runtime_error(what), _offset(offset) {}

decode_exception::decode_exception(decode_exception &&exception, size_t offset)
    : runtime_error(std::move(exception)), _offset(offset) {}

}  // namespace json
}  // namespace spotify
