#pragma once

#include <cstdint>

namespace crystal {

enum class TextureFormat : uint32_t {
  Undefined,
  R8u,
  R8G8u,
  R8G8B8u,
  R8G8B8A8u,
  R8s,
  R8G8s,
  R8G8B8s,
  R8G8B8A8s,
  Depth32f,
};

enum class TextureSample : uint32_t {
  Nearest,
  Linear,
};

enum class TextureRepeat : uint32_t {
  Clamp,
  RepeatX,
  RepeatY,
  RepeatXY,
};

struct TextureDesc {
  uint32_t      width;
  uint32_t      height;
  TextureFormat format;
  TextureSample sample;
  TextureRepeat repeat;
};

}  // namespace crystal
