#pragma once

#include <initializer_list>

namespace crystal {

struct AttachmentDesc {
  bool screen;
  bool clear;
  struct {
    float red;
    float green;
    float blue;
    float alpha;
  } clear_value;
};

struct DepthAttachmentDesc {
  bool  use_depth;
  bool  screen;
  bool  clear;
  float clear_depth;
};

struct RenderPassDesc {
  std::initializer_list<AttachmentDesc> attachments;
  DepthAttachmentDesc                   depth_attachment;
};

}  // namespace crystal
