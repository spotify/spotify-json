# Bazel(http://bazel.io) BUILD file
# external boost libraries

# NOTE: this BUILD file currently does include boost support for Windows

cc_library(
  name = "boost-headers",
  hdrs = glob([
    "boost/**/*.h",
    "boost/**/*pp",
  ]),
  includes = ["."],
  defines = [
    "BOOST_SYSTEM_NO_DEPRECATED",
    "BOOST_THREAD_VERSION=4",
    "BOOST_THREAD_PROVIDES_EXECUTORS",
  ],
)

cc_library(
  name = "atomic",
  deps = ["boost-headers"],
  srcs = ["libs/atomic/src/lockpool.cpp"],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "timer",
  deps = ["boost-headers"],
  srcs = [
    "libs/timer/src/auto_timers_construction.cpp",
    "libs/timer/src/cpu_timer.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "chrono",
  deps = ["boost-headers"],
  srcs = [
    "libs/chrono/src/chrono.cpp",
    "libs/chrono/src/process_cpu_clocks.cpp",
    "libs/chrono/src/thread_clock.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "date_time",
  deps = ["boost-headers"],
  srcs = [
    "libs/date_time/src/gregorian/greg_month.cpp",
    "libs/date_time/src/gregorian/greg_names.hpp",
    "libs/date_time/src/gregorian/greg_weekday.cpp",
    "libs/date_time/src/gregorian/date_generators.cpp",
  ],
  defines = ["DATE_TIME_INLINE"],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "filesystem",
  deps = ["boost-headers"],
  srcs = [
    "libs/filesystem/src/codecvt_error_category.cpp",
    "libs/filesystem/src/operations.cpp",
    "libs/filesystem/src/path.cpp",
    "libs/filesystem/src/path_traits.cpp",
    "libs/filesystem/src/portability.cpp",
    "libs/filesystem/src/unique_path.cpp",
    "libs/filesystem/src/utf8_codecvt_facet.cpp",
    "libs/filesystem/src/windows_file_codecvt.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "program_options",
  deps = ["boost-headers"],
  srcs = [
    "libs/program_options/src/cmdline.cpp",
    "libs/program_options/src/config_file.cpp",
    "libs/program_options/src/options_description.cpp",
    "libs/program_options/src/parsers.cpp",
    "libs/program_options/src/variables_map.cpp",
    "libs/program_options/src/value_semantic.cpp",
    "libs/program_options/src/positional_options.cpp",
    "libs/program_options/src/utf8_codecvt_facet.cpp",
    "libs/program_options/src/convert.cpp",
    "libs/program_options/src/winmain.cpp",
    "libs/program_options/src/split.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "random",
  deps = ["boost-headers"],
  srcs = [
    "libs/random/src/random_device.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "regex",
  deps = ["boost-headers"],
  srcs = [
    "libs/regex/src/c_regex_traits.cpp",
    "libs/regex/src/cpp_regex_traits.cpp",
    "libs/regex/src/cregex.cpp",
    "libs/regex/src/fileiter.cpp",
    "libs/regex/src/icu.cpp",
    "libs/regex/src/instances.cpp",
    "libs/regex/src/posix_api.cpp",
    "libs/regex/src/regex.cpp",
    "libs/regex/src/regex_debug.cpp",
    "libs/regex/src/regex_raw_buffer.cpp",
    "libs/regex/src/regex_traits_defaults.cpp",
    "libs/regex/src/static_mutex.cpp",
    "libs/regex/src/w32_regex_traits.cpp",
    "libs/regex/src/wc_regex_traits.cpp",
    "libs/regex/src/wide_posix_api.cpp",
    "libs/regex/src/winstances.cpp",
    "libs/regex/src/usinstances.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "system",
  deps = ["boost-headers"],
  srcs = [
    "libs/system/src/error_code.cpp",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "thread",
  deps = [
    "boost-headers",
    "chrono",
  ],
  srcs = [
    "libs/thread/src/pthread/thread.cpp",
    "libs/thread/src/pthread/once.cpp",
    "libs/thread/src/future.cpp",
  ],
  defines = [
    "BOOST_THREAD_BUILD_LIB=1",
    "BOOST_THREAD_POSIX",
  ],
  linkopts = [
    "-pthread",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "unit_test_framework",
  deps = ["boost-headers"],
  srcs = [
    "libs/test/src/compiler_log_formatter.cpp",
    "libs/test/src/debug.cpp",
    "libs/test/src/decorator.cpp",
    "libs/test/src/execution_monitor.cpp",
    "libs/test/src/framework.cpp",
    "libs/test/src/plain_report_formatter.cpp",
    "libs/test/src/progress_monitor.cpp",
    "libs/test/src/results_collector.cpp",
    "libs/test/src/results_reporter.cpp",
    "libs/test/src/test_tools.cpp",
    "libs/test/src/test_tree.cpp",
    "libs/test/src/unit_test_log.cpp",
    "libs/test/src/unit_test_main.cpp",
    "libs/test/src/unit_test_monitor.cpp",
    "libs/test/src/unit_test_parameters.cpp",
    "libs/test/src/xml_log_formatter.cpp",
    "libs/test/src/xml_report_formatter.cpp",
  ],
  hdrs = glob([
    "boost/test/*.h",
    "boost/test/*pp",
  ]),
  linkstatic = 1,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "boost",
  deps = [
    "atomic",
    "timer",
    "chrono",
    "date_time",
    "filesystem",
    "program_options",
    "random",
    "regex",
    "system",
    "thread",
  ],
  linkstatic = 1,
  visibility = ["//visibility:public"],
)
