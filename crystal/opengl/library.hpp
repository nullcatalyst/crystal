#pragma once

#include <string>
#include <string_view>

namespace crystal::opengl {

class Context;
class Shader;
class Pipeline;

class Library {
  std::string path_ = {};

public:
  constexpr Library() = default;

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

  Library(const std::string_view path) : path_(path) {}
};

}  // namespace crystal::opengl
