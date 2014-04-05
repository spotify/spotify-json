// Copyright 2014 Felix Bruns and Johan Lindström.

#pragma once

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <stdint.h>

#include "detail/json_macros.hpp"

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
    : _data(static_cast<char *>(::malloc(capacity))),
      _ptr(_data),
      _end(_data + capacity),
      _capacity(capacity) {
    if (!_data) {
      throw std::bad_alloc();
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

  buffer &operator <<(int16_t value) {
    return (value < 0 ?
        write_negative(value) :
        write_positive(static_cast<uint16_t>(value)));
  }

  buffer &operator <<(int32_t value) {
    return (value < 0 ?
        write_negative(value) :
        write_positive(static_cast<uint32_t>(value)));
  }

  buffer &operator <<(int64_t value) {
    return (value < 0 ?
        write_negative(value) :
        write_positive(static_cast<uint64_t>(value)));
  }

  buffer &operator <<(uint16_t value) {
    return write_positive(value);
  }

  buffer &operator <<(uint32_t value) {
    return write_positive(value);
  }

  buffer &operator <<(uint64_t value) {
    return write_positive(value);
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
  json_force_inline void require_bytes(size_t n) {
    if (_ptr + n >= _end) {
      grow_buffer(n);
    }
  }

  json_never_inline void grow_buffer(size_t n) {
    const size_t size(_ptr - _data);
    const size_t new_size(size + n);
    const size_t new_capacity(std::max(new_size, _capacity * 2));

    if (!(_data = static_cast<char *>(realloc(_data, new_capacity)))) {
      throw std::bad_alloc();
    }

    _ptr = _data + size;
    _end = _data + new_capacity;
    _capacity = new_capacity;
  }

  buffer &write_negative(int16_t value) {
    const size_t n(count_digits_negative(value));
    require_bytes(n + 1);
    *_ptr++ = '-';
    switch (n) {
      #define C(_n) case _n: _ptr[_n - 1] = ('0' - (value % 10)); value /= 10
      C( 5); C( 4); C( 3); C( 2); C( 1);
      #undef C
    }
    _ptr += n;
    return *this;
  }

  buffer &write_negative(int32_t value) {
    const size_t n(count_digits_negative(value));
    require_bytes(n + 1);
    *_ptr++ = '-';
    switch (n) {
      #define C(_n) case _n: _ptr[_n - 1] = ('0' - (value % 10)); value /= 10
      C(10); C( 9); C( 8); C( 7); C( 6); C( 5); C( 4); C( 3); C( 2); C( 1);
      #undef C
    }
    _ptr += n;
    return *this;
  }

  buffer &write_negative(int64_t value) {
    const size_t n(count_digits_negative(value));
    require_bytes(n + 1);
    *_ptr++ = '-';
    switch (n) {
      #define C(_n) case _n: _ptr[_n - 1] = ('0' - (value % 10)); value /= 10
      /*20*/ C(19); C(18); C(17); C(16); C(15); C(14); C(13); C(12); C(11);
      C(10); C( 9); C( 8); C( 7); C( 6); C( 5); C( 4); C( 3); C( 2); C( 1);
      #undef C
    }
    _ptr += n;
    return *this;
  }

  buffer &write_positive(uint16_t value) {
    const size_t n(count_digits_positive(value));
    require_bytes(n);
    switch (n) {
      #define C(_n) case _n: _ptr[_n - 1] = ('0' + (value % 10)); value /= 10
      C( 5); C( 4); C( 2); C( 3); C( 1);
      #undef C
    }
    _ptr += n;
    return *this;
  }

  buffer &write_positive(uint32_t value) {
    const size_t n(count_digits_positive(value));
    require_bytes(n);
    switch (n) {
      #define C(_n) case _n: _ptr[_n - 1] = ('0' + (value % 10)); value /= 10
      C(10); C( 9); C( 8); C( 7); C( 6); C( 5); C( 4); C( 3); C( 2); C( 1);
      #undef C
    }
    _ptr += n;
    return *this;
  }

  buffer &write_positive(uint64_t value) {
    const size_t n(count_digits_positive(value));
    require_bytes(n);
    switch (n) {
      #define C(_n) case _n: _ptr[_n - 1] = ('0' + (value % 10)); value /= 10
      C(20); C(19); C(18); C(17); C(16); C(15); C(14); C(13); C(12); C(11);
      C(10); C( 9); C( 8); C( 7); C( 6); C( 5); C( 4); C( 3); C( 2); C( 1);
      #undef C
    }
    _ptr += n;
    return *this;
  }

  size_t count_digits_positive(uint64_t value) {
    return (
        value < 10ULL ? 1 :
        value < 100ULL ? 2 :
        value < 1000ULL ? 3 :
        value < 10000ULL ? 4 :
        value < 100000ULL ? 5 :
        value < 1000000ULL ? 6 :
        value < 10000000ULL ? 7 :
        value < 100000000ULL ? 8 :
        value < 1000000000ULL ? 9 :
        value < 10000000000ULL ? 10 :
        value < 100000000000ULL ? 11 :
        value < 1000000000000ULL ? 12 :
        value < 10000000000000ULL ? 13 :
        value < 100000000000000ULL ? 14 :
        value < 1000000000000000ULL ? 15 :
        value < 10000000000000000ULL ? 16 :
        value < 100000000000000000ULL ? 17 :
        value < 1000000000000000000ULL ? 18 :
        value < 10000000000000000000ULL ? 19 :
        20);
  }

  size_t count_digits_negative(int64_t value) {
    return (
        value > -10LL ? 1 :
        value > -100LL ? 2 :
        value > -1000LL ? 3 :
        value > -10000LL ? 4 :
        value > -100000LL ? 5 :
        value > -1000000LL ? 6 :
        value > -10000000LL ? 7 :
        value > -100000000LL ? 8 :
        value > -1000000000LL ? 9 :
        value > -10000000000LL ? 10 :
        value > -100000000000LL ? 11 :
        value > -1000000000000LL ? 12 :
        value > -10000000000000LL ? 13 :
        value > -100000000000000LL ? 14 :
        value > -1000000000000000LL ? 15 :
        value > -10000000000000000LL ? 16 :
        value > -100000000000000000LL ? 17 :
        value > -1000000000000000000LL ? 18 :
        19);
  }

  size_t count_digits_positive(uint32_t value) {
    return (
        value < 10UL ? 1 :
        value < 100UL ? 2 :
        value < 1000UL ? 3 :
        value < 10000UL ? 4 :
        value < 100000UL ? 5 :
        value < 1000000UL ? 6 :
        value < 10000000UL ? 7 :
        value < 100000000UL ? 8 :
        value < 1000000000UL ? 9 :
        10);
  }

  size_t count_digits_negative(int32_t value) {
    return (
        value > -10L ? 1 :
        value > -100L ? 2 :
        value > -1000L ? 3 :
        value > -10000L ? 4 :
        value > -100000L ? 5 :
        value > -1000000L ? 6 :
        value > -10000000L ? 7 :
        value > -100000000L ? 8 :
        value > -1000000000L ? 9 :
        10);
  }

  size_t count_digits_positive(uint16_t value) {
    return (
        value < 10U ? 1 :
        value < 100U ? 2 :
        value < 1000U ? 3 :
        value < 10000U ? 4 :
        5);
  }

  size_t count_digits_negative(int16_t value) {
    return (
        value > -10 ? 1 :
        value > -100 ? 2 :
        value > -1000 ? 3 :
        value > -10000 ? 4 :
        5);
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
  char *_end;
  size_t _capacity;
};

}  // namespace json
