// Copyright 2014 Felix Bruns and Johan Lindström.

#pragma once

#include <string>
#include <string.h>

namespace json {
namespace detail {

template <typename InputIterator>
inline std::string escape_key(const InputIterator begin, const InputIterator end) {
  static const char *HEX = "0123456789ABCDEF";

  std::string escaped("\"");
  for (InputIterator it = begin; end != it; ++it) {
    const unsigned char ch = static_cast<unsigned char>(*it);
    const bool is_control_character(ch < 0x20);
    const bool is_special_character(ch == '\\' || ch == '"');
    if (is_control_character) {
      escaped.append("\\u00");
      escaped.push_back(HEX[ch >> 4]);
      escaped.push_back(HEX[ch & 0x0F]);
    } else if (is_special_character) {
      escaped.push_back('\\');
      escaped.push_back(ch);
    } else {
      escaped.push_back(ch);
    }
  }

  escaped.push_back('"');
  return escaped;
}

}  // namespace detail

struct key {
  key(const char *raw_value)
      : escaped(detail::escape_key(raw_value, raw_value + ::strlen(raw_value))),
        data(escaped.data()),
        size(escaped.size()) {}

  template <typename Iterable>
  key(const Iterable &iterable)
      : escaped(iterable.begin(), iterable.end()),
        data(escaped.data()),
        size(escaped.size()) {}

  const std::string escaped;
  const char *data;
  const size_t size;
};

}  // namespace json
