// Copyright (c) 2014 Felix Bruns.

#if __APPLE__
#include <chrono>
#else
#include <windows.h>
#endif

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "json_pair.hpp"
#include "json_writer.hpp"

/*#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"
#include "rapidjson/include/rapidjson/filewritestream.h"*/

#if __APPLE__
static uint64_t GetTickCount64() {
  using namespace std::chrono;
  const auto now(steady_clock::now().time_since_epoch());
  return duration_cast<milliseconds>(now).count();
}
#endif

template<typename stream_type>
uint64_t benchmark_json(int iterations, const std::string &filename) {
  stream_type stream;
  json::writer<stream_type> writer(stream);
  uint64_t start = GetTickCount64();

  {
    json::writer<stream_type>::scoped_object root(writer);
    json::writer<stream_type>::scoped_array arr(writer, "arr");
    for (int i = 0; i < iterations; ++i) {
      json::writer<stream_type>::scoped_object obj(writer);
      writer << json::make_pair("key1", "Omgång");
      writer << json::make_pair("key2", 1337);
      writer << json::make_pair("key3", 3.1415962f);
      writer << json::make_pair("key4", 1.0f);
      writer << json::make_pair("key5", true);
      writer << json::make_pair("key6", UINT64_MAX);
    }
  }

  uint64_t time = GetTickCount64() - start;

  std::ofstream file(filename);
  file << std::string(stream.str().data(), stream.str().size());

  return time;
}

/*
template<typename stream_type>
uint64_t benchmark_json_lambda(int iterations, const std::string &filename) {
  stream_type stream;
  json::writer<stream_type> writer(stream);
  uint64_t start = GetTickCount64();

  writer.add_object([&](json::writer<stream_type> &) {
    writer.add_array("arr", [&](json::writer<stream_type> &) {
      for (int i = 0; i < iterations; ++i) {
        writer.add_object([&](json::writer<stream_type> &) {
          writer << json::make_pair("key1", "Omgång");
          writer << json::make_pair("key2", 1337);
          writer << json::make_pair("key3", 3.1415962f);
          writer << json::make_pair("key4", 1.0f);
          writer << json::make_pair("key5", true);
          writer << json::make_pair("key6", UINT64_MAX);
        });
      }
    });
  });

  uint64_t time = GetTickCount64() - start;

  std::ofstream file(filename);
  file << std::string(stream.str().data(), stream.str().size());

  return time;
}
*/
/*uint64_t benchmark_rapidjson(int iterations, const std::string &filename) {
  using namespace rapidjson;

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  uint64_t start = GetTickCount64();
  
  writer.StartObject();
  writer.String("arr");
  writer.StartArray();
  for (int i = 0; i < iterations; ++i) {
    writer.StartObject();

    writer.String("key1");
    writer.String("Omgång");

    writer.String("key2");
    writer.Int(1337);

    writer.String("key3");
    writer.Double(3.1415962f);

    writer.String("key4");
    writer.Double(1.0f);

    writer.String("key5");
    writer.Bool(true);

    writer.String("key6");
    writer.Uint64(UINT64_MAX);

    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
  
  uint64_t time = GetTickCount64() - start;

  std::ofstream file(filename);
  file << std::string(buffer.GetString(), buffer.GetSize());

  return time;
}*/

int main(int argc, char *argv[]) {
  const int iterations = 1 * 1000 * 1000;

  std::cout << "json::buffer => ";
  std::cout << benchmark_json<json::buffer>(iterations, "json_buffer.json") << " ms" << std::endl;

  //std::cout << "json::buffer (lambda) => ";
  //std::cout << benchmark_json_lambda<json::buffer>(iterations, "json_buffer_lambda.json") << " ms" << std::endl;
  
  //std::cout << "std::stringstream => ";
  //std::cout << benchmark_json<std::stringstream>(iterations, "json_buffer_stringstream.json") << " ms" << std::endl;

  //std::cout << "std::stringstream (lambda) => ";
  //std::cout << benchmark_json_lambda<std::stringstream>(iterations, "json_buffer_stringstream_lambda.json") << " ms" << std::endl;

  //std::cout << "rapidjson => ";
  //std::cout << benchmark_rapidjson(iterations, "rapidjson.json") << " ms" << std::endl;

  return 0;
}
