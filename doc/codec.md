codec API
---------

The codec API is designed to make it very easy to turn C++ objects into JSON and
vice versa. It is not a streaming style API like
[yajl](http://lloyd.github.io/yajl/) or SAX, and it is not a DOM-style API that
constructs an abstract syntax tree of a JSON document. Instead, it parses
directly into and writes directly from the C++ objects that are used by the
application.

When using the codec API, the programmer declares the fields of the C++ objects
that should be converted. The declarations are used to both encode and decode
JSON (hence the name "codec"). Because there is only one specification for both
parsing and serialization, there is no risk for that logic to be out of sync.

The `codec`
===========

The main entity of the codec library is, unsurprisingly, the `codec`. Like
iterators in the C++ STL, there is no `codec` class that all `codec`s inherit:
`codec` is a concept. All codecs must expose an `object_type` typedef and
`encode` and `decode` methods. The exact interface is specified in
[codec_interface.hpp](../include/spotify/json/codec/codec_interface.hpp).

`codec`s are highly composable objects. When using the `spotify-json` library,
you will combine the basic `codec`s into increasingly complex ones, until it
can parse the JSON objects that your application works with. The library defines
a number of codecs that are available to the user of the library:

* [`any_t`](#any_t): For type erasing codecs
* [`array_t`](#array_t): For arrays (`std::vector`, `std::deque` etc)
* [`boolean_t`](#boolean_t): For `bool`s
* [`cast_t`](#cast_t): For dynamic casting `shared_ptr`s
* [`equals_t`](#equals_t): For requiring a specific value
* [`lenient_t`](#lenient_t): For allowing values to be of the wrong type
* [`map_t`](#map_t): For `std::map` and other maps
* [`null_t`](#null_t): For `null`
* [`number_t`](#number_t): For parsing numbers (both floating point numbers and
  integers)
* [`object`](#object): For custom C++ objects
* [`one_of_t`](#one_of_t): For trying more than one codec
* [`shared_ptr_t`](#shared_ptr_t): For `shared_ptr`s
* [`string_t`](#string_t): For strings
* [`unique_ptr_t`](#unique_ptr_t): For `unique_ptr`s

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
`codec`, which it uses to parse each element of the array.

The type of `array_codec` in the example below is
`array_t<std::vector<bool>, boolean_t>`, which is quite a mouthful. `codec`
types often become quite involved. `auto` and `decltype` really help when using
this library.

```cpp
const auto array_codec = array<std::vector<bool>>(boolean());
const auto value = decode(array_codec, "[true,false,false]");
```

Following the same principle, `codec`s can be combined to form complex
combinations:

```cpp
using my_type = std::map<std::string,std::unique_ptr<std::string>>;
// Construct a codec for a map from string to shared_ptr of strings,
// where, if an unexpected type is encountered in the value of the map,
// it is ignored instead of failing the parse.
const auto my_codec = map<my_type>(unique_ptr(lenient(string())));
const my_type value = decode(my_codec, "{\"a\":null,\"b\":\"hey\"}");
```

`default_codec_t`
=================

In some cases, it is important to specify exactly how to work with the JSON, but
in many cases there aren't many options: An `std::map<std::string, std::string>`
has one obvious interpretation in JSON, as does `bool` and so on.

As a convenience, `spotify-json` provies an easy way to get a "reasonable
default" `codec` for a given type. It is used as follows:

```cpp
const auto string_codec = default_codec<std::string>();

const auto array_of_bools_codec = default_codec<std::vector<bool>>();

// The types can be arbitrarily deeply nested:
const auto array_of_map_of_array_of_bools_codec = default_codec<
    std::vector<std::map<std::string, std::deque<bool>>>>();
```

It is possible to add support for application specific types to `default_codec`.
This is a common pattern and is the recommended way to use `spotify-json` for
the types that don't require customized behavior depending on the usage. For
example, in the Spotify client, basic types such as `ContextTrack` and `Context`
can be encoded using `default_codec`, while `ContextPlayerState`, which in certain
use cases has to be truncated, has a custom means of getting a `codec` for it.

In order to add support for custom types to `default_codec`, the template
`spotify::json::default_codec_t` should be specialized. Here is an example of
what it usually looks like:

```cpp
struct point {
  int x;
  int y;
}

namespace spotify {
namespace json {

template<>
struct default_codec_t<Point> {
  static object<Point> codec() {
    object<Point> codec;
    codec.required("x", &point::x);
    codec.required("y", &point::y);
    return codec;
  }
};

}  // namespace json
}  // namespace spotify
```

Codecs
======

### `any_t`

`any_t` is a special codec. Its purpose is to *erase* the type of an underlying
codec. This is useful in places where you need to have a codec that encodes and
decodes a specific type of objects, but you cannot know exactly what codec will
be used.

```cpp
// A primitive example of the use of any_t:
const lenient_t<string_t> lenient_codec = lenient(string());
const equals_t<string_t> strict_codec = value("x");

// lenient_codec and strict_codec have separate types, so it's not possible
// to just assign one or the other to another variable. However, both
// lenient_codec and strict_codec can be turned into any_t<std::string>
// objects, which can then be assigned to a variable of that type.
const any_t<std::string> one_or_the_other = strict ?
    any(strict_codec) : any(lenient_codec);
```

The example above is a bit contrived. A more realistic case where use of `any_t`
is required is in pure virtual classes:

```cpp
class my_interface {
  virtual ~my_interface() = default;

  /**
   * Use this codec to encode and decode the my_interface object.
   */
  virtual any_t<std::shared_ptr<my_interface>> codec() = 0;
}
```

In the example above, it is impossible to assign a concrete codec type to the
return type of `my_interface::codec`, since the codec for each implementation
of `my_interface` will have a different type.

Usually in `spotify-json`, there are no virtual method calls. However, `any_t`
introduces one virtual method for each `encode` and `decode` call.

* **Complete class name**: `spotify::json::codec::any_t<ObjectType>`,
  where `ObjectType` is the type of the objects that the codec encodes and
  decodes.
* **Supported types**: `ObjectType`
* **Convenience builder**: `spotify::json::codec::any(InnerCodec)`
* **`default_codec` support**: No; the convenience builder must be used
  explicitly. Unless you know that you need to use this codec, there probably is
  no need to do it.

### `array_t`

`array_t` is a codec for arrays of other values.

* **Complete class name**: `spotify::json::codec::array_t<ArrayType, InnerCodec>`,
  where `ArrayType` is the type of the array, for example `std::vector<int>` or
  `std::list<std::string>`, and `InnerCodec` is the type of the codec that's
  used for the values inside of the array, for example `integer_t` or
  `string_t`.
* **Supported types**: The array-like containers in the STL: `std::vector<T>`,
  `std::list<T>`, `std::deque<T>`, `std::set<T>`, `std::unordered_set<T>`. (When
  parsing into set types, duplicate values are dropped.)
* **Convenience builder**: For example
  `spotify::json::codec::array<std::vector<int>>(integer())`. If no custom inner
  codec is required, `default_codec` is even more convenient.
* **`default_codec` support**: `default_codec<std::vector<T>>()`,
  `default_codec<std::list<T>>()`, `default_codec<std::deque<T>>()`,
  `default_codec<std::set<T>>()`, `default_codec<std::unordered_set<T>>()`

### `boolean_t`

`boolean_t` is a codec for `bool`s.

* **Complete class name**: `spotify::json::codec::bool_t`
* **Supported types**: Only `bool`
* **Convenience builder**: `spotify::json::codec::boolean()`
* **`default_codec` support**: `default_codec<bool>()`


### `cast_t`

`cast_t` is a codec that does `std::dynamic_pointer_cast` on its values. It is
a rather specialized codec that is useful mainly together with `any_t`.

**The use of `cast_t` is not type safe!** It is an error to ask a
`cast_t<T, InnerCodec>` codec to encode an object of any other type than
`InnerCodec::object_type`, even if the object is a `T`. When RTTI is enabled,
doing so will throw an `std::bad_cast`. Otherwise, the behavior is undefined.

Continuing the example given in the documentation for `any_t`, where there is
an interface `my_interface` like this:

```cpp
class my_interface {
 public:
  virtual ~my_interface() = default;

  /**
   * Use this codec to encode and decode the my_interface object.
   */
  virtual any_t<std::shared_ptr<my_interface>> codec() = 0;
}
```

An implementation of this interface might look like:

```cpp
class my_interface_impl : public my_interface {
 public:
  virtual any_t<std::shared_ptr<my_interface>> codec() override {
    object<my_interface_impl> codec;
    codec.required("value", &my_interface_impl::_value);

    return any(cast<my_interface>(shared_ptr(codec)));
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
  explicitly. Unless you know that you need to use this codec, there probably is
  no need to do it.


### `equals_t`

`equals_t` is a codec that only supports encoding and decoding one specific
value. For example, `equals("Hello!")` is a codec that will fail unless the
input is exactly `"Hello!"`.

The most common use case for the `equals_t` codec is to enforce that a version
field in the JSON object has a specific value. It can be combined with
`one_of_t` to construct a codec that supports parsing more than one version of
the object.

```cpp
struct metadata_response {
  std::string name;
}

namespace spotify {
namespace json {

template<>
struct default_codec_t<metadata_response> {
  static one_of_t<object<metadata_response>, object<metadata_response>> codec() {
    object<metadata_response> codec_v1;
    codec.required("n", &metadata_response::x);

    object<metadata_response> codec_v2;
    codec.required("version", equals(2));
    codec.required("name", &metadata_response::x);

    return one_of(codec_v2, codec_v1);
  }
};

}  // namespace json
}  // namespace spotify
```

* **Complete class name**: `spotify::json::codec::equals_t<InnerCodec>`,
  where `InnerCodec` is the type of the codec that actually codes the value.
* **Supported types**: Any type that the underlying codec supports.
* **Convenience builder**: `spotify::json::codec::equals(InnerCodec, Value)`,
  which uses the provided inner codec object, and
  `spotify::json::codec::equals(Value)`, which uses
  `default_codec<Value>()` as the inner codec.
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.


### `lenient_t`

By default, `spotify-json` is strict about the types of the JSON values that it
parses: If it is instructed to read a string but it encounters "null", it will
reject the input. `lenient_t` is a codec that attempts to parse a value using an
inner codec, but if that fails, it skips over the value. It fails only if the
input is malformed JSON.

* **Complete class name**: `spotify::json::codec::lenient_t<InnerCodec>`,
  where `InnerCodec` is the type of the codec that actually codes the value.
* **Supported types**: Any type that the underlying codec supports.
* **Convenience builder**: `spotify::json::codec::lenient(InnerCodec)`
* **`default_codec` support**: No; the convenience builder must be used
  explicitly.


### `map_t`

`map_t` is a codec for maps from string to other values. It only supports
`std::string` keys because that's how JSON is specified. The `map_t` codec is
suitable to use when the maps can contain arbitrary keys. When there is a
pre-defined set of keys that are interesting and any other keys can be
discarded, `object` is more suitable, since it parses the keys directly into a
C++ object in a type-safe way.

* **Complete class name**: `spotify::json::codec::map_t<MapType, InnerCodec>`,
  where `MapType` is the type of the array, for example
  `std::map<std::string, int>` or `std::unordered_map<std::string, bool>`, and
  `InnerCodec` is the type of the codec that's used for the values inside of the
  object, for example `integer_t` or `boolean_t`. The key type of MapType must
  be `std::string`.
* **Supported types**: The map containers in the STL: `std::map<T>` and
  `std::unordered_map<T>`.
* **Convenience builder**: For example
  `spotify::json::codec::map<std::map<std::string, int>>(integer())`. If no
  custom inner codec is required, `default_codec` is even more convenient.
* **`default_codec` support**: `default_codec<std::map<std::string, T>>()`,
  `default_codec<std::unordered_map<std::string, T>>()`.


### `null_t`

`null_t` is a codec that is only capable of parsing and writing the JSON value
null.

* **Complete class name**: `spotify::json::codec::null_t`
* **Supported types**: Only `spotify::json::null_type`
* **Convenience builder**: `spotify::json::codec::null()`
* **`default_codec` support**: `default_codec<null_type>()`


### `number_t`

`number_t` is a codec for numbers, both floating point and integers. Like the
rest of the `spotify-json` library, it uses the
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


### `object`

`object` is arguably the most important codec in `spotify-json`. It is the codec
that parses and writes JSON to and from specific C++ classes and structs. Unlike
the other codecs, `object` codecs aren't created by simply calling a factory
function such as `string()` or `number<float>()`. Instead, an `object<T>` is
created, and then the object is configured for the different fields that exist
in `T`.

For example:

```cpp
struct point {
  int x;
  int y;
}

...

object<point> codec;
codec.required("x", &point::x);
codec.required("y", &point::y);
```

`object<T>` objects have two methods: `required` and `optional`. They have the
exact same method signature. The difference is that fields that were registered
with `required` are required: When an object is being decoded and a required
field is missing from the input, the decoding fails.

For the `required` and `optional` methods, the following overloads exist:

* `optional/required("field_name", &Type::member_pointer, codec)`: Use the
  given codec for encoding and decoding, and the provided member pointer to get
  and assign the value.
* `optional/required("field_name", &Type::member_pointer)`: Use the
  `default_codec<T>()` codec for the type of the member pointer for encoding and
  decoding, and the provided member pointer to get and assign the value.
* `optional/required("field_name", codec)`: When decoding, don't save the
  results anywhere, just make sure that the codec accepts the input. When
  encoding, use a default constructed value of the given type. This is mainly
  useful for verification purposes, for example `required("version", equals(5))`

Each field that `object` encodes and decodes uses one virtual method call.

When encoding, `object` writes fields in the order that they were registered.

It is possible to use `object` for types that are not default constructible, or
when the default constructor does not do the right thing for the use case at
hand. For that, pass in a functor that constructs an object for use in
decoding.

```cpp
struct point {
  point(int x, int y) : x(x), y(y) {}
  int x;
  int y;
}

...

object<point> codec([]{ return point(0, 0); });
codec.required("x", &point::x);
codec.required("y", &point::y);
```

### `one_of_t`

`one_of_t` is a codec that takes one or more inner codecs. When decoding, it
tries the inner codecs one by one until one succeeds to decode. For encoding, it
always uses the first inner codec.

`one_of_t` is useful when there are different versions of the JSON format and
each version has its own codec. A nice pattern is to use [`equals_t`](#equals_t)
in the version-specific codecs to enforce that they only parse JSON it
understands.

* **Complete class name**: `spotify::json::codec::one_of_t<Codec...>`,
  where `Codec...` is a list of the codec types that will be used for encoding
  attempting to decode. All provided codec types must have the same
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
