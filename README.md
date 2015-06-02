spotify-json
============

Fast and nice to use C++ JSON writer and parser library. `spotify-json` itself
is a header-only library, but it depends on
[Google's double-conversion library](https://github.com/google/double-conversion),
which must be linked in to the code that uses `spotify-json`.

Usage
-----

`spotify-json` exposes two different APIs: a high level `codec` API that parses
and writes JSON directly to and from C++ objects, and a low level streaming
style `writer` API. `spotify-json` does not offer a streaming style parsing API.

Since the `codec` API adds parsing support and is more convenient to use, it is
the recommended way to use the API. The `writer` API is only useful in certain
special situations, like when the JSON being written isn't already fully
represented in memory in a C++ object.

### Example

```cpp
#include <iostream>
#include <map>
#include <string>

#include <spotify/json/codec.hpp>

struct Track {
  std::string uri;
  std::string uid;
  std::map<std::string, std::string> metadata;
};

namespace spotify {
namespace json {

// Specialize spotify::json::standard_t to specify default behavior when
// encoding and decoding objects of certain types.
template<>
struct standard_t<Track> {
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

void main() {
  const auto parsed_track = decode<Track>(
      "{\"uri\":\"spotify:track:xyz\",metadata:{\"a\":\"b\"}}");
  std::cout << "Parsed track with uri " << parsed_track.uri << std::endl;

  Track track;
  track.uri = "spotify:track:abc";
  track.uid = "an-uid";
  const auto json = encode(track);
  std::cout << "Encoded the track into " << json << std::endl;
}
```

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
