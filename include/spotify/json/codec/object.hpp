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
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encoding_context.hpp>

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
      typename Create,
      typename = typename std::enable_if<!std::is_same<
          typename std::decay<Create>::type,
          object_t>::value>::type>
  explicit object_t(Create &&create)
      : _construct(std::forward<Create>(create)) {}

  template <typename... Args>
  void optional(const std::string &name, Args &&...args) {
    add_field(name, false, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void required(const std::string &name, Args &&...args) {
    add_field(name, true, std::forward<Args>(args)...);
  }

  object_type decode(decoding_context &context) const {
    uint_fast32_t num_seen_req_fields = 0;
    std::vector<bool> seen_req_fields(_fields.size());

    object_type output = construct(std::is_default_constructible<T>());
    detail::advance_past_object(
        context,
        [](decoding_context &context) {
          static string_t string_decoder;
          return string_decoder.decode(context);
        },
        [&](std::string &&key) {
          const auto field_it = _fields.find(key);
          if (json_unlikely(field_it == _fields.end())) {
            return detail::advance_past_value(context);
          }

          const auto &field = *(*field_it).second;
          field.decode(output, context);
          if (field.required && !seen_req_fields[field.field_id]) {
            seen_req_fields[field.field_id] = true;
            num_seen_req_fields++;
          }
        });

    const auto is_missing_req_fields = (num_seen_req_fields != _num_required_fields);
    detail::fail_if(context, is_missing_req_fields, "Missing required field(s)");
    return output;
  }

  void encode(encoding_context &context, const object_type &value) const {
    context.append('{');
    for (const auto &field : _field_list) {
      field.second->encode(context, field.first, value);
    }
    context.append_or_replace(',', '}');
  }

 private:
  static std::string escape_key(const std::string &key) {
    std::string escaped_key;
    escaped_key += "\"";
    const auto key_begin = reinterpret_cast<const uint8_t *>(key.data());
    detail::write_escaped(escaped_key, key_begin, key_begin + key.size());
    escaped_key += "\":";
    return escaped_key;
  }

  json_force_inline static void append_key_to_context(
      encoding_context &context,
      const std::string &escaped_key) {
    context.append(escaped_key.data(), escaped_key.size());
  }

  template <typename Codec>
  json_force_inline static void append_val_to_context(
      encoding_context &context,
      const Codec &codec,
      const typename Codec::object_type &value) {
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

  struct field {
    field(bool required, size_t field_id)
        : required(required),
          field_id(field_id) {}
    virtual ~field() = default;

    virtual void decode(
        object_type &object,
        decoding_context &context) const = 0;
    virtual void encode(
        encoding_context &context,
        const std::string &escaped_key,
        const object_type &object) const = 0;

    const bool required;
    const size_t field_id;
  };

  template <typename Codec>
  struct dummy_field final : public field {
    dummy_field(bool required, size_t field_id, Codec codec)
        : field(required, field_id),
          codec(std::move(codec)) {}

    void decode(object_type &object, decoding_context &context) const override {
      codec.decode(context);
    }

    void encode(
        encoding_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = typename Codec::object_type();
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    Codec codec;
  };

  template <typename MemberPtr, typename Codec>
  struct member_var_field final : public field {
    member_var_field(bool required, size_t field_id, Codec codec, MemberPtr member_pointer)
        : field(required, field_id),
          codec(std::move(codec)),
          member_pointer(member_pointer) {}

    void decode(object_type &object, decoding_context &context) const override {
      object.*member_pointer = codec.decode(context);
    }

    void encode(
        encoding_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = object.*member_pointer;
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    Codec codec;
    MemberPtr member_pointer;
  };

  template <typename GetterPtr, typename SetterPtr, typename Codec>
  struct member_fn_field final : public field {
    member_fn_field(
        bool required, size_t field_id, Codec codec, GetterPtr getter_ptr, SetterPtr setter_ptr)
        : field(required, field_id),
          codec(std::move(codec)),
          getter_ptr(getter_ptr),
          setter_ptr(setter_ptr) {}

    void decode(object_type &object, decoding_context &context) const override {
      (object.*setter_ptr)(codec.decode(context));
    }

    void encode(
        encoding_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = (object.*getter_ptr)();
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    Codec codec;
    GetterPtr getter_ptr;
    SetterPtr setter_ptr;
  };

  template <typename Getter, typename Setter, typename Codec>
  struct custom_field final : public field {
    template <typename GetterArg, typename SetterArg>
    custom_field(
        bool required, size_t field_id, Codec codec, GetterArg &&get, SetterArg &&set)
        : field(required, field_id),
          codec(std::move(codec)),
          get(std::forward<GetterArg>(get)),
          set(std::forward<SetterArg>(set)) {}

    void decode(object_type &object, decoding_context &context) const override {
      set(object, codec.decode(context));
    }

    void encode(
        encoding_context &context,
        const std::string &escaped_key,
        const object_type &object) const override {
      const auto &value = get(object);
      if (json_likely(detail::should_encode(codec, value))) {
        append_key_to_context(context, escaped_key);
        append_val_to_context(context, codec, value);
      }
    }

    Codec codec;
    Getter get;
    Setter set;
  };

  template <typename ValueType, typename ObjectType>
  void add_field(const std::string &name, bool required, ValueType ObjectType::*member_ptr) {
    add_field(name, required, member_ptr, default_codec<ValueType>());
  }

  template <typename ValueType, typename ObjectType, typename Codec>
  void add_field(const std::string &name,
                 bool required,
                 ValueType ObjectType::*member,
                 Codec &&codec) {
    using MemberPtr = ValueType (ObjectType::*);
    using Field = member_var_field<MemberPtr, typename std::decay<Codec>::type>;
    save_field(
        name,
        required,
        std::make_shared<Field>(required, _fields.size(), std::forward<Codec>(codec), member));
  }

  template <typename GetType, typename SetType, typename GetObjectType, typename SetObjectType>
  void add_field(const std::string &name,
                 bool required,
                 GetType (GetObjectType::*getter)() const,
                 void (SetObjectType::*setter)(SetType)) {
    add_field(name, required, getter, setter, default_codec<typename std::decay<GetType>::type>());
  }

  template <typename GetType,
            typename SetType,
            typename GetObjectType,
            typename SetObjectType,
            typename Codec>
  void add_field(const std::string &name,
                 bool required,
                 GetType (GetObjectType::*getter)() const,
                 void (SetObjectType::*setter)(SetType),
                 Codec &&codec) {
    using GetterPtr = GetType (GetObjectType::*)() const;
    using SetterPtr = void (SetObjectType::*)(SetType);
    using Field = member_fn_field<GetterPtr, SetterPtr, typename std::decay<Codec>::type>;
    save_field(name,
               required,
               std::make_shared<Field>(
                   required, _fields.size(), std::forward<Codec>(codec), getter, setter));
  }

  template <typename Getter, typename Setter>
  void add_field(const std::string &name,
                 bool required,
                 Getter &&getter,
                 Setter &&setter) {
    using ValueType = typename std::decay<decltype(getter(std::declval<T &>()))>::type;
    add_field(name, required, getter, setter, default_codec<ValueType>());
  }

  template <typename Getter, typename Setter, typename Codec>
  void add_field(
      const std::string &name, bool required, Getter &&getter, Setter &&setter, Codec &&codec) {
    using Field = custom_field<typename std::decay<Getter>::type,
                               typename std::decay<Setter>::type,
                               typename std::decay<Codec>::type>;
    save_field(name,
               required,
               std::make_shared<Field>(required,
                                       _fields.size(),
                                       std::forward<Codec>(codec),
                                       std::forward<Getter>(getter),
                                       std::forward<Setter>(setter)));
  }

  template <typename Codec,
            typename = typename std::enable_if<!std::is_member_pointer<Codec>::value>::type>
  void add_field(const std::string &name, bool required, Codec &&codec) {
    using Field = dummy_field<typename std::decay<Codec>::type>;
    save_field(name,
               required,
               std::make_shared<Field>(required, _fields.size(), std::forward<Codec>(codec)));
  }

  void save_field(const std::string &name, bool required, const std::shared_ptr<field> &f) {
    const auto was_saved = _fields.insert(typename field_map::value_type(name, f)).second;
    if (was_saved) {
      _field_list.push_back(std::make_pair(escape_key(name), f));
      if (required) {
        _num_required_fields++;
      }
    }
  }

  using field_vec = std::vector<std::pair<std::string, std::shared_ptr<const field>>>;
  using field_map = std::unordered_map<std::string, std::shared_ptr<const field>>;

  /**
   * _construct may be unset, but only if T is default constructible. This is
   * enforced compile time by enabling the constructor that doesn't set it only
   * if T is default constructible.
   */
  const std::function<T ()> _construct;
  field_vec _field_list;
  field_map _fields;
  size_t _num_required_fields = 0;
};

template <typename T>
object_t<T> object() {
  return object_t<T>();
}

template <typename Create>
auto object(Create &&create) -> object_t<decltype(create())> {
  return object_t<decltype(create())>(std::forward<Create>(create));
}

}  // namespace codec
}  // namespace json
}  // namespace spotify
