#pragma once

#include <initializer_list>

namespace crystal {

union ClearValue {
  float array[4];
  struct {
    float red;
    float green;
    float blue;
    float alpha;
  } color;
  float depth;
};

struct AttachmentDesc {
  bool       clear;
  ClearValue clear_value;
};

}  // namespace crystal
