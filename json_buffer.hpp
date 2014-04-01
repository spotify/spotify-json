// Copyright (c) 2014 Felix Bruns.

#pragma once
#ifndef JSON_BUFFER_HPP_
#define JSON_BUFFER_HPP_

#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

namespace json {

/**
 * \brief Buffer which supports formatting operators and dynamically resizes when needed.
 */
class buffer {
 public:
  /**
   * \brief Create a buffer with the given initial capacity. Default is 4096.
   */
  explicit buffer(size_t capacity = 4096)
    : _data(NULL),
      _ptr(NULL),
      _capacity(capacity) {
    _data = (char *)malloc(_capacity);
    _ptr = _data;

    if (_data == NULL) {
      assert(0);
    }
  }

  /**
   * \brief buffer destructor, which frees internally allocated memory.
   */
  virtual ~buffer() {
    free(_data);
  }

  /**
   * \brief Return a string with the buffer contents.
   *
   * This is mainly here for std::stringstream compatibility when testing...
   */
  std::string str() {
    return std::string(data(), size());
  }

  /**
   * \brief Obtain a pointer to the buffer data.
   */
  const char *data() const {
    return _data;
  }

  /**
   * \brief Returns the size of the buffer data.
   */
  size_t size() const {
    return _ptr - _data;
  }

  /**
   * \brief Returns the current capacity of the buffer.
   */
  size_t capacity() const {
    return _capacity;
  }

  /**
   * \brief Write data to the buffer.
   *
   * This will dynamically grow the buffer.
   */
  buffer &write(const char *s, size_t n) {
    require_bytes(n);
    memcpy(_ptr, s, n);
    _ptr += n;
    return *this;
  }

  /**
   * \brief Write a single byte to the buffer.
   *
   * This will dynamically grow the buffer.
   */
  buffer &put(char c) {
    require_bytes(1);
    *_ptr++ = c;
    return *this;
  }

  buffer &operator <<(char value) {
    return put(value);
  }

  buffer &operator <<(const std::string &value) {
    return write(value.data(), value.size());
  }

  buffer &operator <<(int value) {
    return write_signed<int, unsigned int>(value);
  }

  buffer &operator <<(unsigned int value) {
    return write_unsigned<unsigned int>(value);
  }

  buffer &operator <<(int64_t value) {
    return write_signed<int64_t, uint64_t>(value);
  }

  buffer &operator <<(uint64_t value) {
    return write_unsigned<uint64_t>(value);
  }

  buffer &operator <<(float value) {
    return write_fp<float>(value);
  }

  buffer &operator <<(double value) {
    return write_fp<double>(value);
  }

 private:
  /**
   * \brief Require a given amount of free bytes in the buffer.
   *
   * If there is no space left inside the buffer, this will dynamically allocate more memory.
   */
  void require_bytes(size_t n) {
		size_t size = _ptr - _data;

		if (size + n >= _capacity) {
		  size_t new_capacity = _capacity * 2;
		  size_t new_size = size + n;

		  if (new_capacity < new_size) {
			  new_capacity = new_size;
      }

      _data = (char *)realloc(_data, new_capacity);
      if (_data == NULL) {
        assert(0);
      }

      _ptr = _data + size;
      _capacity = new_capacity;
    }
  }

  template<typename S, typename U>
  buffer &write_signed(S value) {
    if (value < 0) {
      *this << '-';
      value = -value;
    }

    return write_unsigned(static_cast<U>(value));
  }

  template<typename U>
  buffer &write_unsigned(U value) {
    char buffer[20];
    char *buf = buffer;

		do {
			*buf++ = static_cast<char>(value % 10) + '0';
			value /= 10;
		} while (value > 0);

    const int n = buf - buffer;
    require_bytes(n);

    for (int i = 0; i < n; ++i) {
      _ptr[i] = buf[-i-1];
    }

    _ptr += n;

    return *this;
  }

  template<typename D>
  buffer &write_fp(D value) {
    require_bytes(100);

#if _MSC_VER
		_ptr += sprintf_s(_ptr, 100, "%g", value);
#else
		_ptr += snprintf(_ptr, 100, "%g", value);
#endif

    return *this;
  }

  char *_data;
  char *_ptr;
  size_t _capacity;
};

}  // namespace json

#endif  // JSON_BUFFER_HPP_
