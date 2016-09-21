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
