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

#include <array>
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

class cpuid {
 public:
  cpuid() {
#if defined(json_arch_x86)
    const uint32_t cpuid_function(1);
#if defined(_MSC_VER)
    ::__cpuid(reinterpret_cast<int *>(_registers.data()), cpuid_function);
#elif defined(__GNUC__)
    __asm__ __volatile__ (
        "cpuid ;\n"
        : "=a" (_registers[cpu_register::eax]),
          "=b" (_registers[cpu_register::ebx]),
          "=c" (_registers[cpu_register::ecx]),
          "=d" (_registers[cpu_register::edx])
        : "a" (cpuid_function)
        :);
#endif  // defined(_MSC_VER)
#endif  // defined(json_arch_x86)
  }

  bool has_sse42() const {
    return has_feature_bit(cpu_register::ecx, cpu_feature_bit::sse_42);
  }

 private:
  struct cpu_register {
    enum type {
      eax = 0,
      ebx = 1,
      ecx = 2,
      edx = 3
    };
  };

  struct cpu_feature_bit {
    enum type {
      sse_42 = 20,
    };
  };

  bool has_feature_bit(
      const cpu_register::type &reg,
      const cpu_feature_bit::type &bit) const {
    return (_registers[reg] & (1 << bit)) != 0;
  }

  std::array<uint32_t, 4> _registers;
};

}  // detail
}  // json
}  // spotify
