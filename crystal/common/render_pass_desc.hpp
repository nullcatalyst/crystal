#pragma once

#include <initializer_list>

namespace crystal {

struct ColorAttachmentDesc {
  bool clear;
  union {
    float array[4];
    struct {
      float red;
      float green;
      float blue;
      float alpha;
    } color;
  };
};

struct DepthAttachmentDesc {
  bool  clear;
  float depth;
};

}  // namespace crystal
