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

#pragma once

namespace spotify {
namespace json {

/**
 * Overload this template for types that default_codec<T>() should support.
 *
 * The overloaded class should have one static method codec() that returns
 * a codec by value for that type.
 */
template <typename T>
struct default_codec_t;

template <typename T>
decltype(default_codec_t<T>::codec()) default_codec() {
  return default_codec_t<T>::codec();
}

}  // namespace json
}  // namespace spotify
