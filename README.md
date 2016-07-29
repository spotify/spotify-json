spotify-json
============

A C++ JSON writer and parser library. It

* requires very little boilerplate code when using it,
* is fast,
* comes with a good suite of tests,
* is deployed and in active use on well over 100 million devices,
* and [has API documentation](doc/codec.md).

`spotify-json` itself is a header-only library, but it depends on
[Google's double-conversion library](https://github.com/google/double-conversion),
which must be linked in to the code that uses `spotify-json`.

Example
------

```cpp
#include <iostream>
#include <map>
#include <string>

#include <spotify/json.hpp>

struct Track {
  std::string uri;
  std::string uid;
  std::map<std::string, std::string> metadata;
};

namespace spotify {
namespace json {

// Specialize spotify::json::default_codec_t to specify default behavior when
// encoding and decoding objects of certain types.
template<>
struct default_codec_t<Track> {
  static object<Track> codec() {
    object<Track> codec;
    codec.required("uri", &Track::uri);
    codec.optional("uid", &Track::uid);
    codec.optional("metadata", &Track::metadata);
    return codec;
  }
};

}  // namespace json
}  // namespace spotify

int main() {
  const auto parsed_track = decode<Track>(
      "{\"uri\":\"spotify:track:xyz\",metadata:{\"a\":\"b\"}}");
  std::cout << "Parsed track with uri " << parsed_track.uri << std::endl;

  Track track;
  track.uri = "spotify:track:abc";
  track.uid = "an-uid";
  const auto json = encode(track);
  std::cout << "Encoded the track into " << json << std::endl;

  return 0;
}
```

Usage
-----

`spotify-json` offers [a range of `codec` types](doc/codec.md) that can
serialize and parse specific JSON values. There are `codec`s for each of the
basic data types that JSON offers: [strings](doc/codec.md#string_t),
[numbers](doc/codec.md#number_t), [arrays](doc/codec.md#array_t),
[booleans](doc/codec.md#boolean_t), [objects](doc/codec.md#object) and
[null](doc/codec.md#null_t).


### Constructing and composing `codec`s

A `codec` for integers can be made using `codec::integer()`. The codec for
strings can be instantiated with `codec::string()`.

`codec`s are composable. It is for example possible to construct a `codec` for
parsing and serialization of JSON arrays of numbers, such as `[1,4,2]`:
`codec::array<std::vector<int>>(codec::integer())`.

Constructing deeply nested codecs manually as above can become tedious. To ease
this pain, `default_codec` is a helper function that makes it easy to construct
`codec`s for built-in types. For example, `default_codec<int>()` is a codec that
can parse and serialize numbers, and `default_codec<std::vector<int>>()` is one
that works on arrays of numbers.

It is possible to work with JSON objects with arbitrary keys. For example,
`default_codec<std::map<std::string, bool>>()` is a `codec` for JSON objects
with strings as keys and booleans as values.


### Parsing and serialization

Parsing is done using the `decode` function:

```cpp
decode(codec::integer(), "123") == 123;
decode<int>("123") == 123;  // Shortcut for decode(default_codec<int>(), "123")
decode<std::vector<int>>("[1,2,3]") == std::vector{ 1, 2, 3 };
```

Similarly, serialization is done using `encode`: 

```cpp
encode(codec::integer(), 123) == "123";
encode(123) == "123";  // Shortcut for encode(default_codec<int>(), 123)
encode(std::vector<int>{ 1, 2, 3 }) == "[1,2,3]";
```

### Working with rich objects

Working with basic types such as numbers, strings, booleans and arrays is all
nice and dandy, but most practical applications need to deal with rich JSON
schemas that involve objects.

Many JSON libraries work by parsing JSON strings into a tree structure that can
be read by the application. In our experience, this approach often leads to
large amounts of boilerplate code to extract the information in this tree object
into statically typed counterparts that are practical to use in C++. This
boilerplate is painful to write, bug-prone and slow due to unnecessary copying.
SAX-style event based libraries such as [yajl](http://lloyd.github.io/yajl/)
avoid the slowdown but require even more boilerplate.

`spotify-json` avoids these issues by parsing the JSON directly into statically
typed data structures. To explain how, let's use the example of a basic
two-dimensional coordinate, represented in JSON as `{"x":1,"y":2}`. In C++, such
a coordinate may be represented as a struct:

```cpp
struct Coordinate {
  Coordinate() = default;
  Coordinate(int x, int y) : x(x), y(y) {}

  int x = 0;
  int y = 0;
};
```

With `spotify-json`, it is possible to construct a `codec` that can convert
`Coordinate` directly to and from JSON:

```cpp
object<Coordinate> coordinate_codec;
coordinate_codec.required("x", &Coordinate::x);
coordinate_codec.required("y", &Coordinate::y);
```

The use of `required` will cause parsing to fail if the fields are missing.
There is also an `optional` method. For more information, see
[`object`'s API documentation](https://ghe.spotify.net/spotify-sdk/spotify-json/blob/master/doc/codec.md#object).

This `codec` can be used with `encode` and `decode`:

```cpp
encode(coordinate_codec, Coordinate(10, 0)) == "{\"x\":10,\"y\":0}";

const Coordinate coord = decode(coordinate_codec, "{\"x\":12,\"y\":13}");
coord.x == 12;
coord.y == 13;
```

Objects of different types can be nested. To demonstrate this, let's introduce
another data type:

```cpp
struct Player {
  std::string name;
  Coordinate position;
};
```

A `codec` for `Player` might be created with

```cpp
object<Player> player_codec;
player_codec.required("name", &Player::name);
// Because there is no default_codec for Coordinate, we need to pass in the
// codec explicitly:
player_codec.required("position", &Player::position, coordinate_codec);

// Let's use it:
Player player;
player.name = "Daniel";
encode(player_codec, player) == "{\"name\":\"Daniel\",\"position\":{\"x\":0,\"y\":0}}";
```

Since `codec`s are just normal objects, it is possible to create and use
several different `codec`s for any given data type. This makes it possibile to
parameterize parsing and do other fancy things, but for most data types there
will only really exist one `codec`. For these cases, it is possible to extend
the `default_codec` helper to support your own data types.

```cpp
namespace spotify {
namespace json {

template<>
struct default_codec_t<Coordinate> {
  static object<Coordinate> codec() {
    object<Coordinate> codec;
    codec.required("x", &Coordinate::x);
    codec.required("y", &Coordinate::y);
    return codec;
  }
};

template<>
struct default_codec_t<Player> {
  static object<Player> codec() {
    object<Player> codec;
    codec.required("name", &Player::name);
    codec.required("position", &Player::position);
    return codec;
  }
};

}  // namespace json
}  // namespace spotify
```

`Coordinate` and `Player` can now be used like any other type that `spotify-json`
supports out of the box:

```cpp
encode(Coordinate(10, 0)) == "{\"x\":10,\"y\":0}";

decode<std::vector<Coordinate>>("[{\"x\":1,\"y\":-1}]") == std::vector<Coordinate>{ Coordinate(1, -1) };

Player player;
player.name = "Daniel";
encode(player) == "{\"name\":\"Daniel\",\"position\":{\"x\":0,\"y\":0}}";
```


### Advanced uses

The examples above cover the most commonly used parts of `spotify-json`. The
library supports more things that sometimes come in handy:

* Most STL containers, including
  [`vector`, `deque`, `list`, `set`, `unordered_set`](doc/codec.md#array_t),
  [`map` and `unordered_map`](doc/codec.md#map_t)
* [C++ `enum`s and similar types](doc/codec.md#enumeration_t)
* [Arbitrary conversion logic](doc/codec.md#transform_t), for example when a
  raw binary hash in C++ is represented as a hex coded string in JSON
* [Dealing with versioning](doc/codec.md#equals_t)
* [Ignoring values that are of the wrong type instead of failing the parse](doc/codec.md#lenient_t)
* Values wrapped in [`unique_ptr`s](doc/codec.md#unique_ptr_t) and
  [`shared_ptr`s](doc/codec.md#shared_ptr_t)
* [`boost::optional`](doc/codec.md#optional)
* [`boost::chrono` and `std::chrono` types](doc/codec.md#chrono)
* [Dealing with virtual classes / type erasure](doc/codec.md#cast_t)
* [Floating point numbers with lossless serialize/parse roundtrip](doc/codec.md#map_t)


[Detailed API documentation](doc/codec.md)
------------------------------------------

Building and running tests
--------------------------
### Requirements
  * CMake ([http://www.cmake.org](http://www.cmake.org/))
  * Boost ([http://www.boost.org](http://www.boost.org/))

### 1. Make CMake find Boost
    export BOOST_ROOT=/path/to/boost
    export BOOST_LIBRARYDIR=/path/to/boost/lib/

### 2. Run CMake
    mkdir build
    cd build
    cmake -G <generator-name> ..

Run "cmake --help" for a list of generators available on your system.

### 3. Build project with Visual Studio / Xcode / Ninja

### 4. Run CTest
    cd build
    ctest -j 8
