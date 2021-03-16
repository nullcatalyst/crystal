#pragma once

#include <string_view>

#include "crystal/common/proto/proto.hpp"
#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class Shader;
class Pipeline;

class Library {
  OBJC(MTLLibrary) library_ = nullptr;
  common::proto::Library lib_pb_;

public:
  Library() = default;

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

  Library(OBJC(MTLDevice) device, const std::string_view file_path);
};

}  // namespace crystal::metal
