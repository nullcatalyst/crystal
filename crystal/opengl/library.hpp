#pragma once

#include <string>

#include "crystal/opengl/gl.hpp"

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES std::string path_;
#define CRYSTAL_IMPL_CTOR \
  Library(const std::string_view path) : path_(path) {}
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/library.inl"
