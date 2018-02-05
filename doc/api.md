spotify-json API
----------------

The spotify-json API is designed to make it very easy to turn C++ objects into
JSON and vice versa. It is not a streaming style API like
[yajl](http://lloyd.github.io/yajl/) or SAX, and it is not a DOM-style API that
constructs an abstract syntax tree of a JSON document. Instead, it parses
directly into and writes directly from the C++ objects that are used by the
application.

`encode`, `decode` and `try_decode`
===================================

The actual encoding and decoding of JSON is performed by the functions `encode`,
`decode` and `try_decode`. They come in a few varieties, for different use
cases:

### `encode`

```cpp
/**
 * Using a specified codec, encode object to an std::string.
 *
 * @throws encode_exception if the JSON encoding fails, e.g., trying to encode
 * a NaN number or a null pointer.
 */
template <typename Codec>
std::string encode(
    const Codec &codec,
    const typename Codec::object_type &object);

/**
 * Using the default_codec<Value>() codec, encode value to an std::string.
 *
 * This function is a shorthand for: encode(default_codec<Value>(), value)
 *
 * @throws encode_exception if the JSON encoding fails, e.g., trying to encode
 * a NaN number or a null pointer.
 */
template <typename Value>
std::string encode(const Value &value);
```

### `decode`

```cpp
/**
 * Using a specified codec, decode the JSON in string.
 *
 * @throws decode_exception if the JSON parsing fails.
 * @return The parsed object.
 */
template <typename Codec>
typename Codec::object_type decode(
    const Codec &codec,
    const std::string &string);

/**
 * Using a specified codec, decode the JSON in the C style char array data that
 * is size bytes long (not including a \0 at the end).
 *
 * @throws decode_exception if the JSON parsing fails.
 * @return The parsed object.
 */
template <typename Codec>
typename Codec::object_type decode(
    const Codec &codec,
    const char *data,
    size_t size);

/**
 * Using the default_codec<Value>() codec, decode the JSON in string.
 *
 * @throws decode_exception if the JSON parsing fails.
 * @return The parsed object.
 */
template <typename Value>
Value decode(const std::string &string);

/**
 * Using the default_codec<Value>() codec, decode the JSON in the C style char
 * array data that is size bytes long (not including a \0 at the end).
 *
 * @throws decode_exception if the JSON parsing fails.
 * @return The parsed object.
 */
template <typename Value>
Value decode(const char *data, size_t size);
```

### `try_decode`

```cpp
/**
 * Using a specified codec, decode the JSON in string.
 *
 * If the parsing succeeds, the result is assigned to object.
 *
 * @return true if the parsing succeeds.
 */
template <typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const std::string &string);

/**
 * Using a specified codec, decode the JSON in the C style char array data that
 * is size bytes long (not including a \0 at the end).
 *
 * If the parsing succeeds, the result is assigned to object.
 *
 * @return true if the parsing succeeds.
 */
template <typename Codec>
bool try_decode(
    typename Codec::object_type &object,
    const Codec &codec,
    const char *data,
    size_t size);

/**
 * Using the default_codec<Value>() codec, decode the JSON in string.
 *
 * If the parsing succeeds, the result is assigned to object.
 *
 * @return true if the parsing succeeds.
 */
template <typename Value>
bool try_decode(Value &object, const std::string &string);

/**
 * Using the default_codec<Value>() codec, decode the JSON in the C style char
 * array data that is size bytes long (not including a \0 at the end).
 *
 * If the parsing succeeds, the result is assigned to object.
 *
 * @return true if the parsing succeeds.
 */
template <typename Value>
bool try_decode(Value &object, const char *data, size_t size);

/**
 * Using a specified codec, decode the JSON in context. Unlike try_decode, this
 * function allows stray characters after the end of the parsed JSON object.
 *
 * If the parsing succeeds, the result is assigned to object.
 *
 * @return true if the parsing succeeds.
 */
template <typename Codec>
bool try_decode_partial(
    typename Codec::object_type &object,
    const Codec &codec,
    const decode_context &context);
```

`decode_exception`
==================

Exception that is thrown when encoding fails. For more info, see
[decode_exception.hpp](../include/spotify/json/encode_exception.hpp)

`encode_exception`
==================

Exception that is thrown when serialization fails, for example when serializing
a value that has no JSON representation, like NaN floating point numbers. For
more info, see
[encode_exception.hpp](../include/spotify/json/encode_exception.hpp)

Handling missing, empty, `null` and invalid values
==================================================

By default, spotify-json is rather strict in what input it accepts. When parsing
an object of a given type, only JSON input with that type is accepted. For
example, when spotify-json encounters `null` when it expects a string, a number
or an object it fails the parse.

Furthermore, by default spotify-json always encodes fields of objects, even when
they are empty.

This behavior is sometimes not desirable, so spotify-json allows for configuring
it:

* [`object_t`](#object_t) allows the user to declare a field as `optional`. This
  allows the parsing to succeed when a field declared for that object is
  not present. This modifies decoding behavior only.
* [`one_of_t`](#one_of_t) makes it possible to permit `null` or invalid values
  in the input without affecting serialization. This modifies decoding behavior
  only.
* [`empty_as_t`](#empty_as_t) can be used to encode empty values as `null`
  (`empty_as_null`, modifies encoding and decoding behavior) and to omit empty
  values, for example strings or arrays (`empty_as_omit`, modifies encoding
  behavior only).

The codec
=========

The main entity of the spotify-json library is the *codec*. Like iterators in
the C++ STL, there is no codec class that all codecs inherit: codec is a
concept. All codecs must expose an `object_type` typedef and `encode` and
`decode` methods. The exact interface is specified in
[codec_interface.hpp](../include/spotify/json/codec/codec_interface.hpp).

Codecs are highly composable objects. When using the spotify-json library,
you will combine the basic codecs into increasingly complex ones, until it
can parse the JSON objects that your application works with. The library defines
a number of codecs that are available to the user of the library:

* [`any_codec_t`](#any_codec_t): For type erasing codecs
* [`any_value_t`](#any_value_t): For opaque JSON values
* [`array_t`](#array_t): For arrays (`std::vector`, `std::deque` etc)
* [`boolean_t`](#boolean_t): For `bool`s
* [`cast_t`](#cast_t): For dynamic casting `shared_ptr`s
* [`empty_as_t`](#empty_as_t): For controlling encoding behavior of default
  constructed or empty objects.
* [`enumeration_t`](#enumeration_t): For enums and other enumerations of values
* [`eq_t`](#eq_t): For requiring a specific value
* [`ignore_t`](#ignore_t): For ignoring JSON input.
* [`map_t`](#map_t): For `std::map` and other maps
* [`null_t`](#null_t): For `null`
* [`number_t`](#number_t): For parsing numbers (both floating point numbers and
  integers)
* [`object_t`](#object_t): For custom C++ objects
* [`omit_t`](#omit_t): Codec that can't decode and that doesn't encode. For use
  with [`empty_as_t`](#empty_as_t).
* [`one_of_t`](#one_of_t): For trying more than one codec
* [`shared_ptr_t`](#shared_ptr_t): For `shared_ptr`s
* [`string_t`](#string_t): For strings
* [`unique_ptr_t`](#unique_ptr_t): For `unique_ptr`s
* [`transform_t`](#transform_t): For types that the library doesn't have built
  in support for.
* [`tuple_t`](#tuple_t): For `std::pair` and `std::tuple`.
* [`optional_t`](#optional): For `boost::optional`
* [Chrono codecs](#chrono): spotify-json provides support for `std::chrono` and
  `boost::chrono` types.

For example, `boolean_t` is a codec that is capable only of parsing booleans:

```cpp
// Create a boolean_t codec
const boolean_t bool_codec;
// Parse a boolean
const bool value = decode(bool_codec, "true");
// Attempting to parse something else, for example
//   const bool value = decode(bool_codec, "{}");
// fails with a parse error: boolean_t can *only* parse booleans.
```

`array_t` is a more complex codec that is for parsing arrays. It does now know
how to parse the elements that the array contains, so its constructor takes a
codec, which it uses to parse each element of the array.

The type of `array_codec` in the example below is
`array_t<std::vector<bool>, boolean_t>`, which is quite a mouthful. codec
types often become quite involved. `auto` and `decltype` really help when using
this library.

```cpp
const auto array_codec = array<std::vector<bool>>(boolean());
const auto value = decode(array_codec, "[true,false,false]");
```

Following the same principle, codecs can be combined to form complex
combinations:

```cpp
using my_type = std::map<std::string,std::unique_ptr<std::string>>;
// Construct a codec for a map from string to unique_ptr of strings,
// where, if an unexpected type is encountered in the value of the map,
// it is treated as an empty string instead of failing the parse.
const auto my_codec =
    map<my_type>(unique_ptr(one_of(string(), ignore<std::string>())));
const my_type value = decode(my_codec, R"({ "a": null, "b": "hey" })");
```

`default_codec_t`
=================

In some cases, it is important to specify exactly how to work with the JSON, but
in many cases there aren't many options: An `std::map<std::string, std::string>`
has one obvious interpretation in JSON, as does `bool` and so on.

As a convenience, spotify-json provies an easy way to get a "reasonable
default" codec for a given type. It is used as follows:

```cpp
const auto string_codec = default_codec<std::string>();

const auto array_of_bools_codec = default_codec<std::vector<bool>>();

// The types can be arbitrarily deeply nested:
const auto array_of_map_of_array_of_bools_codec = default_codec<
    std::vector<std::map<std::string, std::deque<bool>>>>();
```

It is possible to add support for application specific types to `default_codec`.
This is a common pattern and is the recommended way to use spotify-json for
the types that don't require customized behavior depending on the usage. For
example, in the Spotify client, basic types such as `ContextTrack` and `Context`
can be encoded using `default_codec`, while `ContextPlayerState`, which in certain
use cases has to be truncated, has a custom means of getting codec for it.

In order to add support for custom types to `default_codec`, the template
`spotify::json::default_codec_t` should be specialized. Here is an example of
what it usually looks like:

```cpp
struct Point {
  int x;
  int y;
}

namespace spotify {
namespace json {

template <>
struct default_codec_t<Point> {
  static object_t<Point> codec() {
    auto codec = object<Point>();
    codec.required("x", &Point::x);
    codec.required("y", &Point::y);
    return codec;
  }
};

}  // namespace json
}  // namespace spotify
```

Codecs
======

### `any_codec_t`

`any_codec_t` is a special codec. Its purpose is to *erase* the type of an
underlying codec. This is useful in places where you need to have a codec that
encodes and decodes a specific type of objects, but you cannot know exactly what
codec will be used.

```cpp
// A primitive example of the use of any_codec_t:
const string_t string_codec = string();
const eq_t<string_t> strict_codec = eq("x");

// string_t and eq_t<string_t> have separate types, so it's not possible
// to just assign one or the other to another variable. However, both
// string_t and eq_t<string_t> can be turned into any_codec_t<std::string>
// objects, which can then be assigned to a variable of that type.
const any_codec_t<std::string> one_or_the_other = strict ?
    any_codec(strict_codec) : any_codec(string_codec);
```

The example above is a bit contrived. A more realistic case where use of
`any_codec_t` is required is in pure virtual classes:

```cpp
class my_interface {
  virtual ~my_interface() = default;

  /**
   * Use this codec to encode and decode the my_interface object.
   */
  virtual any_codec_t<std::shared_ptr<my_interface>> codec() = 0;
}
```

In the example above, it is impossible to assign a concrete codec type to the
return type of `my_interface::codec`, since the codec for each implementation
of `my_interface` will have a different type.

Usually in spotify-json, there are no virtual method calls. However,
`any_codec_t` introduces one virtual method for each `encode` and `decode` call.

* **Complete class name**: `spotify::json::codec::any_codec_t<ObjectType>`,
  where `ObjectType` is the type of the objects that the codec encodes and
  decodes.
* **Supported types**: `ObjectType`
* **Convenience builder**: `spotify::json::codec::any_codec(InnerCodec)`
* **`default_codec` support**: No; the convenience builder must be used
  explicitly. Unless you know that you need to use this codec, there probably is
  no need to do it.

### `any_value_t` ###

`any_value_t` is a codec that does not actually decode or encode but instead
deals with opaque JSON values, which can later be decoded by someone else.

When decoding, this codec yields a `spotify::json::encoded_value_ref`, whose
`data()` points to the first character of the value (i.e. `{` for a JSON object
or `t` for the value `true`) and whose `size()` is the size of the entire
encoded value string up to and including the last character (i.e. `}` for a JSON
object or `e` for the value `true`). The `data()` points into the original data
passed to the code so only keep this value around while you can ensure that the
data will outlive the `encoded_value_ref`. If you need the decoded value to live
even when the original JSON has been deleted, store the decoded value in a
`spotify::json::encoded_value` instead (it can be directly assigned from the
`spotify::json::encoded_value_ref`). This should be the default unless you have
special performance requirements and want to take on the extra complexity of
carefully managing the lifetime of the source JSON data.

This codec is useful as it allows you to defer the decoding of certain parts of
your data when decoding. To actually parse the value, use one of the regular
`spotify::decode` functions, passing the `encoded_value`.

### `array_t`

`array_t` is a codec for arrays of other values.

* **Complete class name**: `spotify::json::codec::array_t<ArrayType, InnerCodec>`,
  where `ArrayType` is the type of the array, for example `std::vector<int>` or
  `std::list<std::string>`, and `InnerCodec` is the type of the codec that's
  used for the values inside of the array, for example `integer_t` or
  `string_t`.
* **Supported types**: The array-like containers in the STL:
  `std::array<T, Size>`, `std::vector<T>`, `std::list<T>`, `std::deque<T>`,
  `std::set<T>`, `std::unordered_set<T>`. (When parsing into set types,
  duplicate values are dropped.)
* **Convenience builder**: `spotify::json::codec::array<T>(InnerCodec)`, where
  `T` is the array type. For example
  `spotify::json::codec::array<std::vector<int>>(integer())` or
  or `spotify::json::codec::array<std::set<int>>(integer())`.
  If no custom inner codec is required, `default_codec` is more convenient.
* **`default_codec` support**: `default_codec<std::array<T, Size>>()`,
  `default_codec<std::vector<T>>()`, `default_codec<std::list<T>>()`,
  `default_codec<std::deque<T>>()`, `default_codec<std::set<T>>()`,
  `default_codec<std::unordered_set<T>>()`

### `boolean_t`

`boolean_t` is a codec for `bool`s.

* **Complete class name**: `spotify::json::codec::bool_t`
* **Supported types**: Only `bool`
* **Convenience builder**: `spotify::json::codec::boolean()`
* **`default_codec` support**: `default_codec<bool>()`

### `cast_t`

`cast_t` is a codec that does `std::dynamic_pointer_cast` on its values. It is
a rather specialized codec that is useful mainly together with `any_codec_t`.

**The use of `cast_t` is not type safe!** It is an error to ask a
`cast_t<T, InnerCodec>` codec to encode an object of any other type than
`InnerCodec::object_type`, even if the object is a `T`. When RTTI is enabled,
doing so will throw an `std::bad_cast`. Otherwise, the behavior is undefined.

Continuing the example given in the documentation for `any_codec_t`, where there
is an interface `my_interface` like this:

```cpp
class my_interface {
 public:
  virtual ~my_interface() = default;

  /**
   * Use this codec to encode and decode the my_interface object.
   */
  virtual any_codec_t<std::shared_ptr<my_interface>> codec() = 0;
}
```

An implementation of this interface might look like:

```cpp
class my_interface_impl : public my_interface {
 public:
  virtual any_codec_t<std::shared_ptr<my_interface>> codec() override {
    auto codec = object<my_interface_impl>();
    codec.required("value", &my_interface_impl::_value);
    return any_codec(cast<my_interface>(shared_ptr(codec)));
  }

 private:
  int _value;
};
```

In the code above, the use of `cast_t` is required because when the user of
`my_interface` asks `my_interface_impl`'s codec to encode an
`std::shared_ptr<my_interface>` that pointer has to be cast down to a
`std::shared_ptr<my_interface_impl>`

* **Complete class name**: `spotify::json::codec::cast_t<T, InnerCodec>`,
  where `T` is the type that the `cast_t` codec exposes, and `InnerCodec`
  is the codec that is used for the actual object. When encoding, the `T`
  is `std::dynamic_pointer_cast` to `InnerCodec::object_type`.
* **Supported types**: `std::shared_ptr<T>`
* **Convenience builder**: `spotify::json::codec::cast<T>(InnerCodec)`
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.

### `empty_as_t`

By default, spotify-json never encodes empty smart pointers, `boost::optional`
or arrays as `null`. It also does not accept `null` when decoding unless it has
been explicitly allowed. In many cases, this behavior is fine and expected, but
in some cases it is desirable to be more permissive when parsing, or even
required to emit `null` instead of nothing when an object is missing.

`empty_as_t` is a codec that allows for control over how default constructed
(empty) objects are encoded and decoded. It wraps an inner codec, which does
most of the heavy lifting, and a "default codec", which is used when the object
to be encoded is empty (equal to a default constructed object of that type).

It is most commonly used with the [`null_t`](#null_t) and [`omit_t`](#omit_t)
codecs, and there are convenience functions for those two cases:
`empty_as_null` and `empty_as_omit`.

`empty_as_null` causes empty objects to be encoded as `null`, and causes `null`
to be parsed to a default constructed object. For example,
`empty_as_null(default_codec<std::shared_ptr<std::string>>())` is a codec
for a `shared_ptr<string>` that encodes a `nullptr` `shared_ptr` to `null`.
Without the `empty_as_null` wrapper, null would be disallowed both when
encoding and when decoding.

```cpp
const auto codec = empty_as_null(default_codec<std::shared_ptr<std::string>>());
encode(codec, std::shared_ptr<std::string>()) == "null";
encode(codec, std::make_shared<std::string>("abc")) == "\"abc\"";
```

`empty_as_omit` causes empty objects to be omitted from the output. For
example, `empty_as_omit(string())` is a codec that, when embedded in for
example an `object_t` codec, causes the field to be omitted if it's empty.
This can be useful for decluttering the JSON if the object has many fields that
are usually not set.

```cpp
struct Val {
  std::string a;
  std::string b;
};

// ...

auto codec = object<Val>();
codec.optional("a", &Val::a);
codec.optional("b", &Val::b, empty_as_omit(string()));

encode(codec, Val()) == "{\"a\":\"\"}";  // no "b"
```

* **Complete class name**:
  `spotify::json::codec::default_t<DefaultCodec, InnerCodec>`, where
  `DefaultCodec` is the type of the codec that's used for empty values and
  `InnerCodec` is the type of the codec that's used otherwise.
* **Supported types**: Any default constructible type.
* **Convenience builder**:
  `spotify::json::codec::empty_as_null(inner_codec)`,
  `spotify::json::codec::empty_as_omit(inner_codec)`, or
  `spotify::json::codec::empty_as(default_codec, inner_codec)` to use any
  other codec as default codec.
* **`default_codec` support**: `default_codec<null_type>()`

### `enumeration_t`

`enumeration_t` is a codec that is capable of encoding and decoding a specific
pre-defined set of values. It is useful for enums.

```cpp
enum class Test {
  A,
  B
};

void useEnumeration() {
  using namespace spotify::json;
  using namespace spotify::json::codec;

  const auto codec = enumeration<Test, std::string>({
      { Test::A, "A" },
      { Test::B, "B" } });
  assert(encode(codec, Test::A) == "\"A\"");
  assert(decode(codec, "\"B\"") == Test::B);
}

```

* **Complete class name**:
  `spotify::json::codec::enumeration_t<OuterObject, InnerCodec>`,
  where `InnerCodec` is the type of the codec that actually codes the value
  and `OuterObject` is the type of objects that the codec exposes, often an
  enum type.
* **Supported types**: Any type.
* **Convenience builder**:
  `spotify::json::codec::enumeration<OuterType>(InnerCodec, ListOfValues)`,
  which uses the provided inner codec object with a mapping specified by
  `ListOfValues`. For an example please refer to the example above.
  `spotify::json::codec::enumeration<OuterType, InnerType>(ListOfValues)`,
  like above but uses `default_codec` to create the inner codec.
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.


### `eq_t`

`eq_t` is a codec that only supports encoding and decoding one specific
value. For example, `eq("Hello!")` is a codec that will fail unless the
input is exactly `"Hello!"`.

The most common use case for the `eq_t` codec is to enforce that a version
field in the JSON object has a specific value. It can be combined with
`one_of_t` to construct a codec that supports parsing more than one version of
the object.

```cpp
struct metadata_response {
  std::string name;
}

namespace spotify {
namespace json {

template <>
struct default_codec_t<metadata_response> {
  static one_of_t<object_t<metadata_response>, object_t<metadata_response>> codec() {
    object_t<metadata_response> codec_v1;
    codec_v1.required("n", &metadata_response::name);

    object_t<metadata_response> codec_v2;
    codec_v2.required("version", eq(2));
    codec_v2.required("name", &metadata_response::name);

    return one_of(codec_v2, codec_v1);
  }
};

}  // namespace json
}  // namespace spotify
```

* **Complete class name**: `spotify::json::codec::eq_t<InnerCodec>`,
  where `InnerCodec` is the type of the codec that actually codes the value.
* **Supported types**: Any type that the underlying codec supports.
* **Convenience builder**: `spotify::json::codec::eq(InnerCodec, Value)`,
  which uses the provided inner codec object, and
  `spotify::json::codec::eq(Value)`, which uses
  `default_codec<Value>()` as the inner codec.
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.


### `ignore_t`

`ignore_t` is a primitive codec that just skips over the input JSON and returns
a default constructed object when decoding and that asks to not be encoded. It
can be used for requiring an object to be there when the information in it is
not relevant.

`ignore_t` can be used with the [`one_of_t`](#one_of_t) codec in order to not
fail the parse even if the object does not follow the specified schema. The
documentation for `one_of_t` has examples for how to do this.

[`omit_t`](#omit_t) is similar to `ignore_t`. The difference is that `ignore_t`
always succeeds decoding (if the input is valid JSON) and returns a default
initialized object always fails decoding, while `omit_t` always fails decoding.

* **Complete class name**: `spotify::json::codec::ignore_t<T>`, where `T` is the
  type that is default constructed and returned on decoding.
* **Supported types**: Any copyable type.
* **Convenience builder**: `spotify::json::codec::ignore<T>()`,
  `spotify::json::codec::ignore(value)`.
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.


### `map_t`

`map_t` is a codec for maps from string to other values. It only supports
`std::string` keys because that's how JSON is specified. The `map_t` codec is
suitable to use when the maps can contain arbitrary keys. When there is a
pre-defined set of keys that are interesting and any other keys can be
discarded, `object_t` is more suitable, since it parses the keys directly into a
C++ object in a type-safe way.

* **Complete class name**: `spotify::json::codec::map_t<MapType, InnerCodec>`,
  where `MapType` is the type of the array, for example
  `std::map<std::string, int>` or `std::unordered_map<std::string, bool>`, and
  `InnerCodec` is the type of the codec that's used for the values inside of the
  object, for example `integer_t` or `boolean_t`. The key type of MapType must
  be `std::string`.
* **Supported types**: The map containers in the STL: `std::map<std::string, T>` and
  `std::unordered_map<std::string, T>`. If boost extensions are included, also
  `boost::container::flat_map<std::string, T>`
* **Convenience builder**: For example
  `spotify::json::codec::map<std::map<std::string, int>>(integer())`. If no
  custom inner codec is required, `default_codec` is even more convenient.
* **`default_codec` support**: `default_codec<std::map<std::string, T>>()`,
  `default_codec<std::unordered_map<std::string, T>>()`.

### `null_t`

`null_t` is a codec that is only capable of parsing and writing the JSON value
null. By default it encodes to and from `spotify::json::null_type`, which is an
empty struct class, but it can be used with other types as well.

The `null_t` codec can be used with the [`empty_as_t`](#empty_as_t) codec
in order to encode `nullptr` smart pointers or other empty objects (empty
`boost::optional` or even empty arrays) as `null` in JSON. The documentation for
[`empty_as_t`](#empty_as_t) has examples for how to do this.

* **Complete class name**: `spotify::json::codec::null_t<T>` where `T` is the
  type that is created when the codec successfully decodes a JSON `null`.
* **Supported types**: Any copyable type. `spotify::json::null_type` is used by
  default.
* **Convenience builder**: `spotify::json::codec::null()`,
  `spotify::json::codec::null(value)`; or `spotify::json::codec::null<T>()` to
  use a type other than `null_type`.
* **`default_codec` support**: `default_codec<null_type>()`

### `number_t`

`number_t` is a codec for numbers, both floating point and integers. Like the
rest of the spotify-json library, it uses the
[double-conversion library](https://github.com/google/double-conversion) for
parsing and writing of floats, which means numbers don't drift when being
serialized and then parsed.

* **Complete class name**: `spotify::json::codec::number_t<T>`, where `T` is
  an integral or floating point type.
* **Supported types**: `float`, `double` and integral types such as `int` and
  `size_t`.
* **Convenience builder**: `spotify::json::codec::number<T>()`
* **`default_codec` support**: `default_codec<float>()`,
  `default_codec<double>()`, `default_codec<int>()`,
  `default_codec<size_t>()` etc.


### `object_t`

`object_t` is arguably the most important codec in spotify-json. It is the
codec that parses and writes JSON to and from specific C++ classes and structs.
Unlike the other codecs, `object_t` codecs aren't created by simply calling a
factory function such as `string()` or `number<float>()`. Instead, an
`object_t<T>` is created, and then the object is configured for the different
fields that exist in `T`.

For example:

```cpp
struct Point {
  int x;
  int y;
}

...

auto codec = object<Point>();
codec.required("x", &Point::x);
codec.required("y", &Point::y);
```

`object_t<T>` objects have two methods: `required` and `optional`. They have the
exact same method signature. The difference is that fields that were registered
with `required` are required: When an object is being decoded and a required
field is missing from the input, the decoding fails.

For the `required` and `optional` methods, the following overloads exist:

* `optional/required("field_name", &Type::member_pointer, codec)`: Use the
  given codec for encoding and decoding, and the provided member pointer to get
  and assign the value. `codec` may be omitted in which case
  `default_codec<T>()` for the type of the member variable is used.
* `optional/required("field_name", &Type::get_value, &Type::set_value, codec)`:
  Use the given codec for encoding and decoding and the provided member getter
  and setter pointers to get/set the value. `codec` may be omitted in which
  case `default_codec<T>()` is used where `T` is deduced from the getter.
* `optional/required("field_name", getter, setter, codec)`:
  Uses a pair of callables that act as a custom getter and setter,
  respectively. The getter will be called with the object as a single argument
  and should return the value. The setter will be called with the object as an
  lvalue for the first argument and the value to set as the second (rvalue)
  argument. `codec` may be omitted in which
  case `default_codec<T>()` is used where `T` is deduced from the getter.
* `optional/required("field_name", codec)`: When decoding, don't save the
  results anywhere, just make sure that the codec accepts the input. When
  encoding, use a default constructed value of the given type. This is mainly
  useful for verification purposes, for example `required("version", eq(5))`

Each field that `object_t` encodes and decodes uses one virtual method call.

When encoding, `object_t` writes fields in the order that they were registered.

It is possible to use `object_t` for types that are not default constructible,
or when the default constructor does not do the right thing for the use case at
hand. For that, pass in a functor that constructs an object for use in
decoding.

```cpp
struct Point {
  Point(int x, int y) : x(x), y(y) {}
  int x;
  int y;
}

...

auto codec = object<Point>([]{ return point(0, 0); });
codec.required("x", &Point::x);
codec.required("y", &Point::y);
```

* **Complete class name**: `spotify::json::codec::object_t`
* **Supported types**: Any movable type.
* **Convenience builder**: `spotify::json::codec::object`
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.

### `omit_t`

`omit_t` is a primitive codec that cannot decode any value and that asks to
not be encoded. It indicates that the object should be omitted from the JSON
output. This codec is typically used with the [`empty_as_t`](#empty_as_t)
codec. The documentation for [`empty_as_t`](#empty_as_t) has usage examples.

[`ignore_t`](#ignore_t) is similar to `omit_t`. The difference is that `omit_t`
always fails decoding, while `ignore_t` always succeeds decoding (if the input
is valid JSON) and returns a default initialized object.

* **Complete class name**: `spotify::json::codec::omit_t<T>`, where `T` is the
  type that would have been encoded and decoded if the codec actually did code
  anything.
* **Supported types**: Any type.
* **Convenience builder**: `spotify::json::codec::omit<T>()`
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.

### `one_of_t`

`one_of_t` is a codec that takes one or more inner codecs. When decoding, it
tries the inner codecs one by one until one succeeds to decode. For encoding, it
always uses the first inner codec.

`one_of_t` is useful when there are different versions of the JSON format and
each version has its own codec. A nice pattern is to use [`eq_t`](#eq_t)
in the version-specific codecs to enforce that they only parse JSON it
understands. An example of that can be found on the documentation for the
[`eq_t`](#eq_t) codec.

`one_of_t` can also be used with `null_t` or `ignore_t` to make the decoding
more permissive:

```cpp
struct Val {
  std::string require_string;
  std::string allow_null;
  std::string allow_anything;
};

// ...

auto codec = object<Val>();
codec.required("require_string", &Val::require_string);
// If parsing the value as a string fails, try parsing null
codec.required("allow_null", &Val::b, one_of(string(), null<std::string>()));
// If parsing the value as a string fails, try skipping over
// one JSON value in the input and parse as "".
codec.required("allow_anything", &Val::b, one_of(string(), ignore<std::string>()));

const auto val = decode<Val>(
    "{\"require_string\":\"a\",\"allow_null\":null,\"allow_anything\":123}");
val.require_string == "a";
val.allow_null == "";
val.allow_anything == "";

```

* **Complete class name**: `spotify::json::codec::one_of_t<Codec...>`,
  where `Codec...` is a list of the codec types that will be used for encoding
  and decoding. All provided codec types must have the same
  `object_type`. (It is for example illegal to create a
  `one_of_t<string_t, boolean_t>`).
* **Supported types**: Any type that the underlying codecs support.
* **Convenience builder**: `spotify::json::codec::one_of(Codec...)`
* **`default_codec` support**: No; the convenience builder must be used explicitly.

### `shared_ptr_t`

`shared_ptr_t` is a codec that wraps and unwraps values in a `std::shared_ptr`.
Like the other "container" codecs, it doesn't know how to code what it contains;
that is done by a codec inside it.

* **Complete class name**: `spotify::json::codec::shared_ptr_t<InnerCodec>`,
  where `InnerCodec` is the type of the codec that actually codes the value.
* **Supported types**: `std::shared_ptr<T>`, where `T` is move or copy
  constructible.
* **Convenience builder**: `spotify::json::codec::shared_ptr(InnerCodec)`
* **`default_codec` support**: `default_codec<shared_ptr<T>>()`


### `string_t`

`string_t` is a codec for strings.

* **Complete class name**: `spotify::json::codec::string_t`
* **Supported types**: Only `std::string`
* **Convenience builder**: `spotify::json::codec::string()`
* **`default_codec` support**: `default_codec<std::string>()`


### `unique_ptr_t`

`unique_ptr_t` is a codec that wraps and unwraps values in a `std::unique_ptr`.
Like the other "container" codecs, it doesn't know how to code what it contains;
that is done by a codec inside it.

* **Complete class name**: `spotify::json::codec::unique_ptr_t<InnerCodec>`,
  where `InnerCodec` is the type of the codec that actually codes the value.
* **Supported types**: `std::unique_ptr<T>`, where `T` is move or copy
  constructible.
* **Convenience builder**: `spotify::json::codec::unique_ptr(InnerCodec)`
* **`default_codec` support**: `default_codec<unique_ptr<T>>()`


### `transform_t`

There are types that spotify-json doesn't have built-in support for that aren't
objects that should be encoded as JSON objects (this case is handled by
object_t). For these types, transform_t can be used. transform_t codecs are
created with a pair of conversion functions: One that transforms an object that
is about to be encoded into a type that another encoder can handle, and another
that transforms an object that has just been decoded back to the outwards facing
type.

This can be used for example when encoding a SHA1 hash which is encoded in
JSON as a string.

For example:

```cpp
struct my_type {
  std::string value;
};

...
// Codec is for my_type C++ objects and encodes/decodes them as JSON strings.
const auto codec = transform(
    [](const my_type &object) {
      return object.value;
    },
    [](const std::string &value) {
      return my_type{ value };
    });
```

* **Complete class name**: `spotify::json::codec::transform_t<
  InnerCodec, EncodeTransform, DecodeTransform>`, where `InnerCodec` is the type
  of the codec that actually codes the value, `EncodeTransform` is the type of
  a function or functor that takes an object of a type (call it `T`) and
  returns an `InnerCodec::object_type`, and `DecodeTransform` is the type of a
  function or functor that takes an `InnerCodec::object_type` and returns a `T`.
* **Supported types**: Any type that is move or copy constructible.
* **Convenience builder**: `spotify::json::codec::transform(InnerCodec,
  EncodeTransform, DecodeTransform)`, and
  `spotify::json::codec::transform(EncodeTransform, DecodeTransform)`, which
  uses the default codec for the inner type.
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.


### `tuple_t`

`tuple_t` is a codec for `std::pair` and `std::tuple`. It encodes the object as
fixed-size JSON arrays. It can be used for JSON arrays where the type of all the
objects are not the same.

Examples:

```cpp
const auto coord = decode<std::tuple<int, int, int>>("[1,3,2]");
```

```cpp
struct Command {
  // ...
};

const auto command_with_sequence_number =
    decode<std::pair<size_t, Command>>("[1337,{}]");
```

```cpp
struct Point {
  Point() = default;
  Point(int x, int y) : x(x), y(y) {}

  int x = 0;
  int y = 0;
};

auto codec = transform(
    pair(number<int>(), number<int>()),
    [](const Point &p) { return make_pair(p.x, p.y); },
    [](const std::pair<int, int> &p) { return Point(p.first, p.second); });

const auto point = decode(codec, "[1,3]");
```

* **Complete class name**: `spotify::json::codec::tuple_t<
  T, Codec...>`, where `T` is the type of the encoded pair or tuple, `Codec...`
  is a list of the codec types that will be used for encoding and decoding.
* **Supported types**: `std::pair` and `std::tuple`
* **Convenience builder**: `spotify::json::codec::pair(CodecA, CodecB)` and
  `spotify::json::codec::tuple(Codec...)`.
* **`default_codec` support**: `default_codec<std::pair<A, B>>()` and
  `default_codec<std::tuple<T...>>()`


### `optional_t`

`optional_t` is a codec for strings for `boost::optional<T>`. Uninitialized
values (equivalent to `boost::none`) are not encoded at all (`should_encode()`
returns `false`). To encode `boost::none` as `null`, use the
[`empty_as_t`](#empty_as_t) codec, e.g., `spotify::json::empty_as_null()`.

This codec is in header `<spotify/json/boost.hpp>`.

* **Complete class name**: `spotify::json::codec::optional_t`
* **Supported types**: `std::optional<T>`
* **Convenience builder**: `spotify::json::codec::optional`
* **`default_codec` support**: `default_codec<boost::optional<T>>()`

### chrono

spotify-json provides support for `duration` and `time_point` types of
`std::chrono` and `boost::chrono`. They are implemented using `transform_t`, so
they don't have `*_t` classes like many of the other codecs.

For `boost::chrono`, include header `<spotify/json/boost.hpp>`.

* **Complete class name**: N/A. See above.
* **Supported types**: All `std::chrono::duration`, `std::chrono::time_point`,
  `boost::chrono::duration` and `boost::chrono::time_point` types, including
  `std::chrono::system_clock::duration`, `std::chrono::system_clock::time_point`
  etc.
* **Convenience builder**: `spotify::json::codec::duration<Duration>()` and
  `spotify::json::codec::time_point<TimePoint>()`
* **`default_codec` support**:
  `default_codec<std::chrono::duration<Rep, Period>>()`,
  `default_codec<boost::chrono::duration<Rep, Period>>()`,
  `default_codec<std::chrono::time_point<Clock, Duration>>()`,
  `default_codec<boost::chrono::time_point<Clock, Duration>>()`
