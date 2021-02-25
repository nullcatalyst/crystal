#pragma once

#include <initializer_list>

namespace crystal {

struct ColorAttachmentDesc {
  bool clear;
  struct {
    float red;
    float green;
    float blue;
    float alpha;
  } clear_value;
};

struct DepthAttachmentDesc {
  bool  clear;
  float clear_depth;
};

}  // namespace crystal
