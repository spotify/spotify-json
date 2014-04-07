// Copyright (c) 2014 Felix Bruns.

#pragma once

#include <functional>
#include <map>
#include <stdint.h>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "detail/json_escape.hpp"
#include "detail/json_locale.hpp"

#include "json_buffer.hpp"
#include "json_key.hpp"
#include "json_pair.hpp"

namespace json {

struct null_type {};
static null_type null;

template<typename stream_type>
class basic_writer {
 public:
  explicit basic_writer(stream_type &stream)
    : _stream(stream),
      _separator_needed(false),
      _scoped_locale(LC_NUMERIC_MASK, "C") {}

  virtual ~basic_writer() {}

  template<typename T>
  basic_writer &operator <<(const T &value) {
    return separator_and_set().write(value);
  }

  basic_writer &operator <<(const null_type &) {
    return separator_and_set().write("null", 4);
  }

  basic_writer &operator <<(bool value) {
    return (value ?
        separator_and_set().write("true", 4) :
        separator_and_set().write("false", 5));
  }

  basic_writer &operator <<(int8_t value) {
    return separator_and_set().write(static_cast<signed>(value));
  }

  basic_writer &operator <<(uint8_t value) {
    return separator_and_set().write(static_cast<unsigned>(value));
  }

  basic_writer &operator <<(const key &key) {
    return separator_and_set().write(key.data, key.size);
  }

  basic_writer &operator <<(const char *value) {
    separator_and_set().put('"');
    detail::write_escaped(_stream, value, detail::null_terminated_end_iterator());
    return put('"');
  }

  basic_writer &operator <<(const std::string &value) {
    separator_and_set().put('"');
    detail::write_escaped(_stream, value.begin(), value.end());
    return put('"');
  }

  template<typename K, typename V>
  basic_writer &operator <<(const pair<K, V> &pair) {
    return (*this << pair.key).clear_separator().put(':') << pair.value;
  }

  template<typename T, typename U>
  basic_writer &operator <<(const std::pair<T, U> &pair) {
    return (*this << pair.first).clear_separator().put(':') << pair.second;
  }

  template<typename T>
  basic_writer &operator <<(const std::vector<T> &vector) {
    const scoped_array array(*this);
    typedef typename std::vector<T>::const_iterator const_iterator;
    for (const_iterator it = vector.begin(), end = vector.end(); it != end; ++it) {
      *this << *it;
    }
    return *this;
  }

  template<typename T>
  basic_writer &operator <<(const std::set<T> &set) {
    const scoped_array array(*this);
    typedef typename std::vector<T>::const_iterator const_iterator;
    for (const_iterator it = set.begin(), end = set.end(); it != end; ++it) {
      *this << *it;
    }
    return *this;
  }

  template<typename K, typename V>
  basic_writer &operator <<(const std::map<K, V> &map) {
    const scoped_object object(*this);
    typedef typename std::map<K, V>::const_iterator const_iterator;
    for (const_iterator it = map.begin(), end = map.end(); it != end; ++it) {
      *this << *it;
    }
    return *this;
  }

  /**
   * \brief Helper class for writing JSON arrays to a writer.
   */
  class scoped_array {
   public:
    explicit scoped_array(basic_writer &writer)
        : _writer(writer) {
      _writer.separator_and_clear().put('[');
    }

    scoped_array(basic_writer &writer, const char *key)
        : _writer(writer) {
      (_writer.separator_and_clear() << key).clear_separator().put(':').put('[');
    }

    virtual ~scoped_array() {
      _writer.put(']').set_separator();
    }

   private:
    basic_writer &_writer;
  };

  /**
   * \brief Write a JSON array to the underlying stream.
   */
  void add_array(const std::function<void(writer &)> &func) {
    const scoped_array object(*this);
    func(*this);
  }

  /**
   * \brief Write a keyed JSON array to the underlying stream.
   */
  void add_array(const char *key, const std::function<void(writer &)> &func) {
    const scoped_array array(*this, key);
    func(*this);
  }

  /**
   * \brief Helper class for writing JSON objects to a writer.
   */
  class scoped_object {
   public:
    explicit scoped_object(basic_writer &writer)
        : _writer(writer) {
      _writer.separator_and_clear().put('{');
    }

    scoped_object(basic_writer &writer, const char *key)
        : _writer(writer) {
      (_writer.separator_and_clear() << key).clear_separator().put(':').put('{');
    }

    virtual ~scoped_object() {
      _writer.put('}').set_separator();
    }

   private:
    basic_writer &_writer;
  };

  /**
   * \brief Write a JSON object to the underlying stream.
   */
  void add_object(const std::function<void(writer &)> &func) {
    const scoped_object object(*this);
    func(*this);
  }

  /**
   * \brief Write a keyed JSON object to the underlying stream.
   */
  void add_object(const char *key, const std::function<void(writer &)> &func) {
    const scoped_object object(*this, key);
    func(*this);
  }

 private:
  basic_writer &separator_and_clear() {
    if (_separator_needed) {
      _stream << ',';
    }
    _separator_needed = false;
    return *this;
  }

  basic_writer &separator_and_set() {
    if (_separator_needed) {
      _stream << ',';
    }
    _separator_needed = true;
    return *this;
  }

  basic_writer &clear_separator() {
    _separator_needed = false;
    return *this;
  }

  basic_writer &set_separator() {
    _separator_needed = true;
    return *this;
  }

  /**
   * \brief Write a value to the underlying stream.
   */
  template<typename T>
  basic_writer &write(const T &value) {
    _stream << value;
    return *this;
  }

  /**
   * \brief Write raw data to the underlying stream.
   */
  basic_writer &write(const char *s, size_t n) {
    _stream.write(s, n);
    return *this;
  }

  /**
   * \brief Write a single character to the underlying stream.
   */
  basic_writer &put(char c) {
    _stream.put(c);
    return *this;
  }

  /**
   * \brief Underlying stream.
   */
  stream_type &_stream;

  /**
   * \brief Current separator flag.
   */
  bool _separator_needed;

  /**
   * \brief Scoped locale change.
   */
  detail::scoped_locale _scoped_locale;
};

typedef basic_writer<buffer> writer;

}  // namespace json
