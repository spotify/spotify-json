/*
 * Copyright (c) 2015-2019 Spotify AB
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

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

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
namespace codec_detail {

struct object_t_base {
  ~object_t_base();

 protected:
  struct construct_untyped {
    virtual ~construct_untyped() = default;
  };

  object_t_base();
  object_t_base(construct_untyped *construct);
  object_t_base(object_t_base &&other);
  object_t_base(const object_t_base &other);

  void decode(decode_context &context, void *value) const;
  void encode(encode_context &context, const void *value) const;

  detail::field_registry _fields;

  /**
   * _construct may be unset, but only if T is default constructible. This is
   * enforced compile time by enabling the constructor that doesn't set it only
   * if T is default constructible.
   */
  std::shared_ptr<const construct_untyped> _construct;
};

}  // namespace codec_detail

template <typename T>
class object_t final : public codec_detail::object_t_base {
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
      : object_t_base(new construct_function<create_function>(std::move(create))) {}

  template <typename... args_type>
  void optional(const std::string &name, args_type &&...args) {
    add_field(name, false, std::forward<args_type>(args)...);
  }

  template <typename... args_type>
  void required(const std::string &name, args_type &&...args) {
    add_field(name, true, std::forward<args_type>(args)...);
  }

  json_never_inline object_type decode(decode_context &context) const {
    object_type value = construct(std::is_default_constructible<T>());
    object_t_base::decode(context, &value);
    return value;
  }

  json_force_inline void encode(encode_context &context, const object_type &value) const {
    object_t_base::encode(context, &value);
  }

 private:
  T construct(std::true_type /*is_default_constructible*/) const {
    if (json_unlikely(_construct)) {
      const auto &typed = static_cast<const construct_callable &>(*_construct);
      return typed();
    } else {
      return object_type();
    }
  }

  T construct(std::false_type /*is_default_constructible*/) const {
    // T is not default constructible. Because _construct must be set if T is
    // not default constructible, there is no reason to test it in this case.
    return (*_construct)();
  }

  template <typename codec_type>
  struct codec_field : public detail::field {
    codec_field(bool required, size_t required_field_idx, codec_type &&codec)
        : field(required, required_field_idx),
          codec(std::move(codec)) {}

    codec_field(bool required, size_t required_field_idx, const codec_type &codec)
        : field(required, required_field_idx),
          codec(codec) {}

    template <typename value_type>
    void append_kv(encode_context &context, const std::string &key, const value_type &value) const {
      if (json_likely(detail::should_encode(this->codec, value))) {
        context.append(key.data(), key.size());
        this->codec.encode(context, value);
        context.append(',');
      }
    }

    codec_type codec;
  };

  template <typename codec_type>
  struct dummy_field final : public codec_field<codec_type> {
    dummy_field(bool required, size_t required_field_idx, codec_type &&codec)
        : codec_field<codec_type>(required, required_field_idx, std::move(codec)) {}

    dummy_field(bool required, size_t required_field_idx, const codec_type &codec)
        : codec_field<codec_type>(required, required_field_idx, codec) {}

    void decode(decode_context &context, void *) const override {
      this->codec.decode(context);
    }

    void encode(encode_context &context, const std::string &key, const void *) const override {
      this->append_kv(context, key, typename codec_type::object_type());
    }
  };

  template <typename member_ptr, typename codec_type>
  struct member_var_field final : public codec_field<codec_type> {
    member_var_field(bool required, size_t required_field_idx, codec_type &&codec, member_ptr member)
        : codec_field<codec_type>(required, required_field_idx, std::move(codec)),
          member(member) {}

    member_var_field(bool required, size_t required_field_idx, const codec_type &codec, member_ptr member)
        : codec_field<codec_type>(required, required_field_idx, codec),
          member(member) {}

    void decode(decode_context &context, void *object) const override {
      auto &typed = *static_cast<object_type *>(object);
      typed.*member = this->codec.decode(context);
    }

    void encode(encode_context &context, const std::string &key, const void *object) const override {
      const auto &typed = *static_cast<const object_type *>(object);
      const auto &value = typed.*member;
      this->append_kv(context, key, value);
    }

    member_ptr member;
  };

  template <typename getter_ptr, typename setter_ptr, typename codec_type>
  struct member_fn_field final : public codec_field<codec_type> {
    member_fn_field(bool required, size_t required_field_idx, codec_type &&codec, getter_ptr getter, setter_ptr setter)
        : codec_field<codec_type>(required, required_field_idx, std::move(codec)),
          getter(getter),
          setter(setter) {}

    member_fn_field(bool required, size_t required_field_idx, const codec_type &codec, getter_ptr getter, setter_ptr setter)
        : codec_field<codec_type>(required, required_field_idx, codec),
          getter(getter),
          setter(setter) {}

    void decode(decode_context &context, void *object) const override {
      auto &typed = *static_cast<object_type *>(object);
      (typed.*setter)(this->codec.decode(context));
    }

    void encode(encode_context &context, const std::string &key, const void *object) const override {
      const auto &typed = *static_cast<const object_type *>(object);
      const auto &value = (typed.*getter)();
      this->append_kv(context, key, value);
    }

    getter_ptr getter;
    setter_ptr setter;
  };

  template <typename getter, typename setter, typename codec_type>
  struct custom_field final : public codec_field<codec_type> {
    template <typename getter_arg, typename setter_arg>
    custom_field(bool required, size_t required_field_idx, codec_type &&codec, getter_arg &&get, setter_arg &&set)
        : codec_field<codec_type>(required, required_field_idx, std::move(codec)),
          get(std::forward<getter_arg>(get)),
          set(std::forward<setter_arg>(set)) {}

    template <typename getter_arg, typename setter_arg>
    custom_field(bool required, size_t required_field_idx, const codec_type &codec, getter_arg &&get, setter_arg &&set)
        : codec_field<codec_type>(required, required_field_idx, codec),
          get(std::forward<getter_arg>(get)),
          set(std::forward<setter_arg>(set)) {}

    void decode(decode_context &context, void *object) const override {
      auto &typed = *static_cast<object_type *>(object);
      set(typed, this->codec.decode(context));
    }

    void encode(encode_context &context, const std::string &key, const void *object) const override {
      const auto &typed = *static_cast<const object_type *>(object);
      const auto &value = get(typed);
      this->append_kv(context, key, value);
    }

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
    _fields.save(name, required, std::shared_ptr<detail::field>(new field_type(
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
    _fields.save(name, required, std::shared_ptr<detail::field>(new field_type(
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
    _fields.save(name, required, std::shared_ptr<detail::field>(new field_type(required,
        _fields.num_required_fields(),
        std::forward<codec_type>(codec),
        std::forward<getter>(get),
        std::forward<setter>(set))));
  }

  template <
      typename codec_type,
      typename = typename std::enable_if<!std::is_member_pointer<codec_type>::value>::type>
  void add_field(const std::string &name, bool required, codec_type &&codec) {
    using field_type = dummy_field<typename std::decay<codec_type>::type>;
    _fields.save(name, required, std::shared_ptr<detail::field>(new field_type(
        required,
        _fields.num_required_fields(),
        std::forward<codec_type>(codec))));
  }

  struct construct_callable : public construct_untyped {
    virtual T operator()() const = 0;
  };

  template <typename function_type>
  struct construct_function final : public construct_callable {
    construct_function(function_type &&fn) : _fn(fn) {}

    T operator()() const override {
      return _fn();
    }

    const function_type _fn;
  };
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
