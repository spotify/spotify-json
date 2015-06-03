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
* [`smart_ptr_t`](#smart_ptr_t): For `shared_ptr`s and `unique_ptr`s
* [`string_t`](#string_t): For strings

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

`standard_t`
===========

In some cases, it is important to specify exactly how to work with the JSON, but
in many cases there aren't many options: An `std::map<std::string, std::string>`
has one obvious interpretation in JSON, as does `bool` and so on.

As a convenience, `spotify-json` provies an easy way to get a "reasonable
default" `codec` for a given type. It is used as follows:

```cpp
const auto string_codec = standard<std::string>();

const auto array_of_bools_codec = standard<std::vector<bool>>();

// The types can be arbitrarily deeply nested:
const auto array_of_map_of_array_of_bools_codec = standard<
    std::vector<std::map<std::string, std::deque<bool>>>>();
```

It is possible to add support for application specific types to `standard`. This
is a common pattern and is the recommended way to use `spotify-json` for the
types that don't require customized behavior depending on the usage. For
example, in the Spotify client, basic types such as `ContextTrack` and `Context`
can be encoded using `standard`, while `ContextPlayerState`, which in certain
use cases has to be truncated, has a custom means of getting a `codec` for it.

In order to add support for custom types to `standard`, the template
`spotify::json::standard_t` should be specialized. Here is an example of what it
usually looks like:

```cpp
struct Point {
  int x;
  int y;
}

namespace spotify {
namespace json {

template<>
struct standard_t<Point> {
  static object<Point> codec() {
    object<Point> codec;
    codec.required("x", &Point::x);
    codec.optional("y", &Point::y);
    return codec;
  }
};

}  // namespace json
}  // namespace spotify
```

Codecs
======

### `any_t`


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
  codec is required, `standard` is even more convenient.
* **`standard` support**: `standard<std::vector<T>>()`,
  `standard<std::list<T>>()`, `standard<std::deque<T>>()`,
  `standard<std::set<T>>()`, `standard<std::unordered_set<T>>()`

### `boolean_t`

`boolean_t` is a codec for `bool`s.

* **Complete class name**: `spotify::json::codec::bool_t`
* **Supported types**: Only `bool`
* **Convenience builder**: `spotify::json::codec::boolean()`
* **`standard` support**: `standard<bool>()`


### `cast_t`


### `equals_t`


### `lenient_t`


### `map_t`


### `null_t`

`null_t` is a codec that is only capable of parsing and writing the JSON value
null.

* **Complete class name**: `spotify::json::codec::null_t`
* **Supported types**: Only `spotify::json::null_type`
* **Convenience builder**: `spotify::json::codec::null()`
* **`standard` support**: `standard<null_type>()`


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
* **`standard` support**: `standard<float>()`, `standard<double>()`,
  `standard<int>()`, `standard<size_t>()` etc.


### `object`


### `one_of_t`


### `smart_ptr_t`


### `string_t`

`string_t` is a codec for strings.

* **Complete class name**: `spotify::json::codec::string_t`
* **Supported types**: Only `std::string`
* **Convenience builder**: `spotify::json::codec::string()`
* **`standard` support**: `standard<std::string>()`
