spotify-json
============

A C++ JSON writer and parser library. It

* requires very little boilerplate code when using it,
* is fast,
* comes with a good suite of tests,
* is deployed and in active use on well over 100 million devices.
* [has documentation](doc/codec.md).

`spotify-json` itself is a header-only library, but it depends on
[Google's double-conversion library](https://github.com/google/double-conversion),
which must be linked in to the code that uses `spotify-json`.

Usage
-----

`spotify-json` exposes a high level `codec` API that parses and writes JSON
directly to and from C++ objects. `spotify-json` does not offer a streaming API
or a DOM style API.

[Detailed API documentation](doc/codec.md)

### Example

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
