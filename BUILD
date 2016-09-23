# Copyright (c) 2016 Spotify AB
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# Bazel(http://bazel.io) BUILD file
# spotify-json library

cc_library(
  name = "spotify-json",
  deps = [
    "//external:double-conversion",
  ],
  srcs = glob(["src/**/*.cpp"]),
  hdrs = glob(["include/spotify/json/**/*.hpp"]),
  includes = ["include"],
  visibility = ["//visibility:public"],
  copts = ["-msse4.2"]
)

cc_test(
  name = "benchmark",
  deps = [
    ":spotify-json",
    "@boost//:boost",
    "@boost//:unit_test_framework",
  ],
  srcs = glob([
    "benchmark/src/*.cpp",
    "benchmark/include/spotify/json/benchmark/**/*.hpp"
  ]),
  includes = ["benchmark/include"],
  visibility = ["//visibility:private"]
)

cc_test(
  name = "test",
  deps = [
    ":spotify-json",
    "@boost//:boost",
    "@boost//:unit_test_framework",
  ],
  srcs = glob([
    "test/src/*.cpp",
    "test/include/spotify/json/test/**/*.hpp"
  ]),
  includes = ["test/include"],
  visibility = ["//visibility:private"]
)
