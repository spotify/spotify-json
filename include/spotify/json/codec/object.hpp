/*
 * Copyright (c) 2015-2016 Spotify AB
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

#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <spotify/json/codec/number.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/decode_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/bitset.hpp>
#include <spotify/json/detail/field_registry.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/detail/skip_value.hpp>
#include <spotify/json/encode_context.hpp>

namespace spotify {
namespace json {
namespace codec {

template <typename T>
class object_t final {
 public:
  using object_type = T;

  template <
      typename U = T,
      typename = typename std::enable_if<std::is_default_constructible<U>::value>::type>
  object_t() {}

  object_t(const object_t<T> &) = default;
  object_t(object_t<T> &&) = default;

  template <
      typename create_function,
      typename = typename std::enable_if<!std::is_same<
          typename std::decay<create_function>::type,
          object_t>::value>::type>
  explicit object_t(create_function &&create)
      : _construct(std::forward<create_function>(create)) {}

  template <typename... args_type>
  void optional(const std::string &name, args_type &&...args) {
    add_field(name, false, std::forward<args_type>(args)...);
  }

  template <typename... args_type>
  void required(const std::string &name, args_type &&...args) {
    add_field(name, true, std::forward<args_type>(args)...);
  }

  json_never_inline object_type decode(decode_context &context) const {
    uint_fast32_t uniq_seen_required = 0;
    detail::bitset<64> seen_required(_fields.num_required_fields());

    object_type output = construct(std::is_default_constructible<T>());
    detail::decode_object<string_t>(context, [&](const std::string &key) {
      const auto *field_ptr = _fields.find(key);
      if (json_unlikely(!field_ptr)) {
        return detail::skip_value(context);
      }

      const auto &field = from_field_storage_ptr(field_ptr);
      field.decode(context, output);
      if (field.is_required()) {
        const auto seen = seen_required.test_and_set(field.required_field_idx());
        uniq_seen_required += (1 - seen);  // 'seen' is 1 when the field is a duplicate; 0 otherwise
      }
    });

    const auto is_missing_req_fields = (uniq_seen_required != _fields.num_required_fields());
    detail::fail_if(context, is_missing_req_fields, "Missing required field(s)");
    return output;
  }

  void encode(encode_context &context, const object_type &value) const {
    context.append('{');
    for (const auto &field_storage : _fields) {
      const auto &field = from_field_storage_ptr(field_storage.second.get());
      field.encode(context, field_storage.first, value);
    }
    context.append_or_replace(',', '}');
  }

 private:
  json_force_inline static void append_key_to_context(
      encode_context &context,
      const std::string &escaped_key) {
    context.append(escaped_key.data(), escaped_key.size());
  }

  template <typename codec_type, typename value_type>
  json_force_inline static void append_val_to_context(
      encode_context &context,
      const codec_type &codec,
      const value_type &value) {
    codec.encode(context, value);
    context.append(',');
  }

  T construct(std::true_type is_default_constructible) const {
    // Avoid the cost of an std::function invocation if no construct function
    // is provided.
    return _construct ? _construct() : object_type();
  }

  T construct(std::false_type is_default_constructible) const {
    // T is not default constructible. Because _construct must be set if T is
    // not default constructible, there is no reason to test it in this case.
    return _construct();
  }

  struct field : public detail::field_base {
    field(bool required, size_t required_field_idx)
        : _data(required ? required_field_idx : json_size_t_max) {}
    virtual void decode(decode_context &context, object_type &object) const = 0;
    virtual void encode(
        encode_context &context,
        const std::string &escaped_key,
        const object_type &object) const = 0;

    json_force_inline bool is_required() const { return (_data != json_size_t_max); }
    json_force_inline size_t required_field_idx() const { return _data; }

   private:
    size_t _data;
  };

  static const field &from_field_storage_ptr(const void *field_storage) {
    return *static_cast<const field *>(field_storage);
  }

  template <typename codec_type>
  struct dummy_field final : public field {
    dummy_field(bool required, size_t required_field_idx, codec_type codec)
        : field(required, required_field_idx),
          codec(std::move(codec)) {}

    void decode(decode_context &context, object_type &object) const override {
      codec.decode(context);
    }

    void encode(
        encode_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = typename codec_type::object_type();
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    codec_type codec;
  };

  template <typename member_ptr, typename codec_type>
  struct member_var_field final : public field {
    member_var_field(bool required, size_t required_field_idx, codec_type codec, member_ptr member)
        : field(required, required_field_idx),
          codec(std::move(codec)),
          member(member) {}

    void decode(decode_context &context, object_type &object) const override {
      object.*member = codec.decode(context);
    }

    void encode(
        encode_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = object.*member;
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    codec_type codec;
    member_ptr member;
  };

  template <typename getter_ptr, typename setter_ptr, typename codec_type>
  struct member_fn_field final : public field {
    member_fn_field(
        bool required, size_t required_field_idx, codec_type codec, getter_ptr getter, setter_ptr setter)
        : field(required, required_field_idx),
          codec(std::move(codec)),
          getter(getter),
          setter(setter) {}

    void decode(decode_context &context, object_type &object) const override {
      (object.*setter)(codec.decode(context));
    }

    void encode(
        encode_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = (object.*getter)();
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    codec_type codec;
    getter_ptr getter;
    setter_ptr setter;
  };

  template <typename getter, typename setter, typename codec_type>
  struct custom_field final : public field {
    template <typename getter_arg, typename setter_arg>
    custom_field(
        bool required, size_t required_field_idx, codec_type codec, getter_arg &&get, setter_arg &&set)
        : field(required, required_field_idx),
          codec(std::move(codec)),
          get(std::forward<getter_arg>(get)),
          set(std::forward<setter_arg>(set)) {}

    void decode(decode_context &context, object_type &object) const override {
      set(object, codec.decode(context));
    }

    void encode(
        encode_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = get(object);
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    codec_type codec;
    getter get;
    setter set;
  };

  template <typename value_type, typename object_type>
  void add_field(const std::string &name, bool required, value_type object_type::*member_ptr) {
    add_field(name, required, member_ptr, default_codec<value_type>());
  }

  template <typename value_type, typename object_type, typename codec_type>
  void add_field(
      const std::string &name,
      bool required,
      value_type object_type::*member,
      codec_type &&codec) {
    using member_ptr = value_type (object_type::*);
    using field_type = member_var_field<member_ptr, typename std::decay<codec_type>::type>;
    _fields.save(name, required, std::shared_ptr<detail::field_base>(new field_type(
        required,
        _fields.num_required_fields(),
        std::forward<codec_type>(codec),
        member)));
  }

  template <
      typename get_type,
      typename set_type,
      typename get_object_type,
      typename set_object_type>
  void add_field(
      const std::string &name,
      bool required,
      get_type (get_object_type::*getter)() const,
      void (set_object_type::*setter)(set_type)) {
    add_field(name, required, getter, setter, default_codec<typename std::decay<get_type>::type>());
  }

  template <
      typename get_type,
      typename set_type,
      typename get_object_type,
      typename set_object_type,
      typename codec_type>
  void add_field(
      const std::string &name,
      bool required,
      get_type (get_object_type::*getter)() const,
      void (set_object_type::*setter)(set_type),
      codec_type &&codec) {
    using getter_ptr = get_type (get_object_type::*)() const;
    using setter_ptr = void (set_object_type::*)(set_type);
    using field_type = member_fn_field<getter_ptr, setter_ptr, typename std::decay<codec_type>::type>;
    _fields.save(name, required, std::shared_ptr<detail::field_base>(new field_type(
        required,
        _fields.num_required_fields(),
        std::forward<codec_type>(codec),
        getter,
        setter)));
  }

  template <typename getter, typename setter>
  void add_field(
      const std::string &name,
      bool required,
      getter &&get,
      setter &&set) {
    using value_type = typename std::decay<decltype(get(std::declval<T &>()))>::type;
    add_field(name, required, get, set, default_codec<value_type>());
  }

  template <typename getter, typename setter, typename codec_type>
  void add_field(const std::string &name, bool required, getter &&get, setter &&set, codec_type &&codec) {
    using field_type = custom_field<
        typename std::decay<getter>::type,
        typename std::decay<setter>::type,
        typename std::decay<codec_type>::type>;
    _fields.save(name, required, std::shared_ptr<detail::field_base>(new field_type(required,
        _fields.num_required_fields(),
        std::forward<codec_type>(codec),
        std::forward<getter>(get),
        std::forward<setter>(set))));
  }

  template <typename codec_type,
            typename = typename std::enable_if<!std::is_member_pointer<codec_type>::value>::type>
  void add_field(const std::string &name, bool required, codec_type &&codec) {
    using field_type = dummy_field<typename std::decay<codec_type>::type>;
    _fields.save(name, required, std::shared_ptr<detail::field_base>(new field_type(
        required,
        _fields.num_required_fields(),
        std::forward<codec_type>(codec))));
  }

  /**
   * _construct may be unset, but only if T is default constructible. This is
   * enforced compile time by enabling the constructor that doesn't set it only
   * if T is default constructible.
   */
  const std::function<T ()> _construct;
  detail::field_registry _fields;
};

template <typename T>
object_t<T> object() {
  return object_t<T>();
}

template <typename create_function>
auto object(create_function &&create) -> object_t<decltype(create())> {
  return object_t<decltype(create())>(std::forward<create_function>(create));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
