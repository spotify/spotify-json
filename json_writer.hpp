// Copyright (c) 2014 Felix Bruns.

#pragma once
#ifndef JSON_WRITER_HPP_
#define JSON_WRITER_HPP_

#include <functional>
#include <map>
#include <stdint.h>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "json_escape.hpp"
#include "json_key.hpp"
#include "json_locale.hpp"
#include "json_pair.hpp"
#include "json_buffer.hpp"

namespace json {

struct null_type {};
static null_type null;

template<typename stream_type>
class writer {
 public:
  explicit writer(stream_type &stream)
    : _separator_needed(false),
      _stream(stream),
      _scoped_locale(LC_NUMERIC_MASK, "C") {}

  virtual ~writer() {}

  template<typename T>
  writer &operator <<(const T &value) {
    return separator().write(value).set_separator(true);
  }

  writer &operator <<(const null_type &) {
    return separator().write("null", 4).set_separator(true);
  }

  writer &operator <<(bool value) {
    return separator().write(value ? "true" : "false", value ? 4 : 5).set_separator(true);
  }

  writer &operator <<(int8_t value) {
    return separator().write(static_cast<signed>(value)).set_separator(true);
  }

  writer &operator <<(uint8_t value) {
    return separator().write(static_cast<unsigned>(value)).set_separator(true);
  }

  writer &operator <<(const key &key) {
    return separator().write(key.data, key.size).set_separator(true);
  }

  writer &operator <<(const char *value) {
    separator().put('"');
    write_escaped(_stream, value, null_terminated_end_iterator());
    return put('"').set_separator(true);
  }

  writer &operator <<(const std::string &value) {
    separator().put('"');
    write_escaped(_stream, value.begin(), value.end());
    return put('"').set_separator(true);
  }

  template<typename K, typename V>
  writer &operator <<(const pair<K, V> &pair) {
    return (*this << pair.key).set_separator(false).put(':') << pair.value;
  }

  template<typename T, typename U>
  writer &operator <<(const std::pair<T, U> &pair) {
    return (*this << pair.first).set_separator(false).put(':') << pair.second;
  }

  template<typename T>
  writer &operator <<(const std::vector<T> &vector) {
    scoped_array arr(*this);
    for (typename std::vector<T>::const_iterator it = vector.begin(); it != vector.end(); ++it) {
      *this << *it;
    }
    return *this;
  }

  template<typename T>
  writer &operator <<(const std::set<T> &set) {
    scoped_array arr(*this);
    for (typename std::vector<T>::const_iterator it = set.begin(); it != set.end(); ++it) {
      *this << *it;
    }
    return *this;
  }

  template<typename K, typename V>
  writer &operator <<(const std::map<K, V> &map) {
    scoped_object obj(*this);
    for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it) {
      *this << *it;
    }
    return *this;
  }

  /**
   * \brief Helper class for writing JSON arrays to a writer.
   */
  class scoped_array {
   public:
    explicit scoped_array(writer &writer)
      : _writer(writer) {
      _writer.separator().put('[');
    }

    scoped_array(writer &writer, const char *key)
      : _writer(writer) {
      (_writer.separator() << key).set_separator(false).put(':').put('[');
    }

    virtual ~scoped_array() {
      _writer.set_separator(false).put(']').set_separator(true);
    }

   private:
    writer &_writer;
  };

  /**
   * \brief Write a JSON array to the underlying stream.
   */
  void add_array(const std::function<void(writer &)> &func) {
    scoped_array obj(*this);
    func(*this);
  }

  /**
   * \brief Write a keyed JSON array to the underlying stream.
   */
  void add_array(const char *key, const std::function<void(writer &)> &func) {
    scoped_array arr(*this, key);
    func(*this);
  }

  /**
   * \brief Helper class for writing JSON objects to a writer.
   */
  class scoped_object {
   public:
    explicit scoped_object(writer &writer)
      : _writer(writer) {
      _writer.separator().put('{');
    }

    scoped_object(writer &writer, const char *key)
      : _writer(writer) {
      (_writer.separator() << key).set_separator(false).put(':').put('{');
    }

    virtual ~scoped_object() {
      _writer.set_separator(false).put('}').set_separator(true);
    }

   private:
    writer &_writer;
  };

  /**
   * \brief Write a JSON object to the underlying stream.
   */
  void add_object(const std::function<void(writer &)> &func) {
    scoped_object obj(*this);
    func(*this);
  }

  /**
   * \brief Write a keyed JSON object to the underlying stream.
   */
  void add_object(const char *key, const std::function<void(writer &)> &func) {
    scoped_object obj(*this, key);
    func(*this);
  }

 private:
  /**
   * \brief Conditionally write a separator to the underlying stream.
   */
  writer &separator() {
    if (_separator_needed) {
      _stream << ',';
    }
    return set_separator(false);
  }

  /**
   * \brief Set the separator flag of this writer.
   */
  writer &set_separator(bool needed) {
    _separator_needed = needed;
    return *this;
  }

  /**
   * \brief Write a value to the underlying stream.
   */
  template<typename T>
  writer &write(const T &value) {
    _stream << value;
    return *this;
  }

  /**
   * \brief Write raw data to the underlying stream.
   */
  writer &write(const char *s, size_t n) {
    _stream.write(s, n);
    return set_separator(false);
  }

  /**
   * \brief Write a single character to the underlying stream.
   */
  writer &put(char c) {
    _stream.put(c);
    return set_separator(false);
  }

  /**
   * \brief Current separator flag.
   */
  bool _separator_needed;

  /**
   * \brief Underlying stream.
   */
  stream_type &_stream;

  /**
   * \brief Scoped locale change.
   */
  scoped_locale _scoped_locale;
};

}  // namespace json

#endif  // JSON_WRITER_HPP_
