#pragma once

namespace crystal {

namespace opengl {
class Context;
}  // namespace opengl

namespace vulkan {
class Context;
}  // namespace vulkan

}  // namespace crystal

#if CRYSTAL_USE_OPENGL
#include "crystal/opengl/context.hpp"
#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN
#include "crystal/vulkan/context.hpp"
#endif  // ^^^ CRYSTAL_USE_VULKAN
