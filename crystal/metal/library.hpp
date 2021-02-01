#pragma once

#include <string_view>

#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class Shader;
class Pipeline;

class Library {
  OBJC(MTLLibrary) library_ = nullptr;

public:
  constexpr Library() = default;

  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;

  Library(Library&& other) = default;
  Library& operator=(Library&& other) = default;

  ~Library() = default;

  void destroy() noexcept { library_ = nullptr; }

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::Shader;
  friend class ::crystal::metal::Pipeline;

  Library(OBJC(MTLDevice) device, const std::string_view path);
};

}  // namespace crystal::metal
