#pragma once

#include <cstdio>

#include "glad_gl42/glad.h"
#undef APIENTRY

#include "util/msg/msg.hpp"

#ifdef CRYSTAL_RELEASE

#define GL_ASSERT($call, $msg, ...) ($call)

#else

#define GL_ASSERT($call, $msg, ...)                                                              \
  do {                                                                                           \
    ($call);                                                                                     \
    GLenum $result = glGetError();                                                               \
    if ($result != GL_NO_ERROR) {                                                                \
      const char* $err = glResultToString($result);                                              \
      util::msg::fatal(stderr, __FILE__, ":", __LINE__, ": (", $err, ") ", $msg, ##__VA_ARGS__); \
    }                                                                                            \
  } while (0)

#endif  // CRYSTAL_RELEASE

inline const char* glResultToString(GLenum result) {
  const char* err_msg = "<unknown>";
  switch (result) {
#define X(x)      \
  case x:         \
    err_msg = #x; \
    break;
#include "crystal/opengl/gl_error.inl"
#undef X
    default:
      break;
  }
  return err_msg;
}
