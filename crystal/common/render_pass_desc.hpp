#pragma once

#include <initializer_list>

namespace crystal {

struct AttachmentDesc {
  bool clear;
  union {
    float array[4];
    struct {
      float red;
      float green;
      float blue;
      float alpha;
    } color;
    float depth;
  };
};

}  // namespace crystal
