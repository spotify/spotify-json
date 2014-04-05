// Copyright 2014 Felix Bruns and Johan Lindström.

#pragma once

#include <string>

#include "detail/json_escape.hpp"

namespace json {

class key {
 public:
  explicit key(const char *raw)
      : _storage("\""),
        _ref(detail::write_escaped(_storage, raw, detail::null_terminated_end_iterator())),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

  template<typename Iterable>
  explicit key(const Iterable &iterable)
      : _storage("\""),
        _ref(detail::write_escaped(_storage, iterable.begin(), iterable.end())),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

 private:
  std::string _storage;
  std::string &_ref;

 public:
  const char *data;
  const size_t size;
};

}  // namespace json
