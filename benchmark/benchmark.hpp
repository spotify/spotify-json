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

#include <chrono>
#include <iostream>

template <typename test_fn>
void benchmark(const char *name, const size_t count, const test_fn &test) {
  using namespace std::chrono;
  const auto before = high_resolution_clock::now();
  for (unsigned i = 0; i < count; i++) {
    test();
  }
  const auto after = high_resolution_clock::now();

  const auto duration = (after - before);
  const auto duration_us = duration_cast<microseconds>(duration).count();
  const auto duration_us_avg = (duration_us / static_cast<double>(count));
  std::cerr << name << ": " << duration_us_avg << " us avg (" << count << " runs)" << std::endl;
}

#define JSON_BENCHMARK(n, test) benchmark(typeid(*this).name(), (n), (test))
