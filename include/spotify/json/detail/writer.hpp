/*
 * Copyright (c) 2014 Spotify AB
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

#include <stdint.h>
#include <string>

#include <spotify/json/buffer.hpp>
#include <spotify/json/detail/escape.hpp>
#include <spotify/json/detail/key.hpp>

namespace spotify {
namespace json {
namespace detail {

struct null_type {};
static null_type null;

struct null_options_type {};

template<typename stream_type, typename options_type>
class basic_writer {
 public:
  explicit basic_writer(stream_type &stream)
    : _stream(stream),
      _separator_needed(false) {}

  basic_writer(stream_type &stream, const options_type &options)
    : _stream(stream),
      _options(options),
      _separator_needed(false) {}

  virtual ~basic_writer() {}

  basic_writer &operator <<(const options_type &options) {
    _options = options;
    return *this;
  }

  const options_type &options() const {
    return _options;
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
    write_escaped(_stream, value, null_terminated_end_iterator());
    return put('"');
  }

  basic_writer &operator <<(char *value) {
    return *this << const_cast<const char *>(value);
  }

  basic_writer &operator <<(const std::string &value) {
    separator_and_set().put('"');
    write_escaped(_stream, value.begin(), value.end());
    return put('"');
  }

  basic_writer &operator <<(const buffer &buffer) {
    return separator_and_set().write(buffer.data(), buffer.size());
  }

  /**
   * \brief Write a JSON key to the underlying stream. This prints the key and
   * then a colon, so it must be followed by a call to print the value that the
   * key should refer to. It must be called while printing the keys of an
   * object.
   */
  template<typename K>
  basic_writer &add_key(const K &key) {
    return (*this << key).clear_separator().put(':');
  }

  /**
   * \brief Write a JSON key and value to the underlying stream. It must be
   * called while printing the keys of an object.
   */
  template<typename K, typename V>
  basic_writer &add_pair(const K &key, const V &value) {
    return add_key(key) << value;
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

    template<typename KeyType>
    scoped_array(basic_writer &writer, const KeyType &key)
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
  template<typename Functor>
  void add_array(const Functor &func) {
    const scoped_array object(*this);
    func(*this);
  }

  /**
   * \brief Write a keyed JSON array to the underlying stream.
   */
  template<typename KeyType, typename Functor>
  void add_array(const KeyType &key, const Functor &func) {
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

    template<typename KeyType>
    scoped_object(basic_writer &writer, const KeyType &key)
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
  template<typename Functor>
  void add_object(const Functor &func) {
    const scoped_object object(*this);
    func(*this);
  }

  /**
   * \brief Write a keyed JSON object to the underlying stream.
   */
  template<typename KeyType, typename Functor>
  void add_object(const KeyType &key, const Functor &func) {
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
   * \brief Writer options. This is a template parameter and can for instance
   * be used for excluding certain fields from the output.
   */
  options_type _options;

  /**
   * \brief Current separator flag.
   */
  bool _separator_needed;

  /**
   * Give the operator overload below access to our internals.
   */
  template<typename S, typename O, typename T>
  friend basic_writer<S, O> &operator <<(basic_writer<S, O> &, const T &);
};

/**
 * Declared out-of-line in order to avoid greedy template matching.
 * By declaring this template here the compiler will consider other
 * possibly matching overloads before settling on this one.
 */
template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const T &value) {
  return writer.separator_and_set().write(value);
}

typedef basic_writer<buffer, null_options_type> writer;

}  // namespace detail
}  // namespace json
}  // namespace spotify
