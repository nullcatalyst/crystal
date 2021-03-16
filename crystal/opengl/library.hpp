#pragma once

#include <string_view>

#include "crystal/common/proto/proto.hpp"

namespace crystal::opengl {

class Context;
class Shader;
class Pipeline;

class Library {
  common::proto::Library lib_pb_;

public:
  Library() = default;

  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;

  Library(Library&& other) = default;
  Library& operator=(Library&& other) = default;

  ~Library() = default;

  void destroy() noexcept {}

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::Shader;
  friend class ::crystal::opengl::Pipeline;

  Library(const std::string_view file_path);
};

}  // namespace crystal::opengl
