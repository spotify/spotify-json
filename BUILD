# Bazel(http://bazel.io) BUILD file
# spotify-json library

cc_library(
  name = "spotify-json",
  deps = [
    "//external:double-conversion",
  ],
  hdrs = glob(["include/spotify/json/**/*.hpp"]),
  includes = ["include"],
  visibility = ["//visibility:public"],
)

cc_test(
  name = "benchmark",
  deps = [
    ":spotify-json",
    "@boost//:boost",
    "@boost//:unit_test_framework",
  ],
  srcs = glob([
    "benchmark/*.hpp",
    "benchmark/*.cpp",
  ]),
  includes = ["benchmark"],
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
    "test/*.hpp",
    "test/*.cpp",
  ]),
  includes = ["test"],
  visibility = ["//visibility:private"]
)