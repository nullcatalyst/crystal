#pragma once

#if __cpp_concepts >= 201907
#define CRYSTAL_HAS_CONCEPTS 1
#include "crystal/concepts/context.hpp"
#endif  // ^^^ __cpp_concepts >= 201907

namespace crystal {

namespace opengl {
class Context;
}  // namespace opengl

namespace vulkan {
class Context;
}  // namespace vulkan

namespace metal {
class Context;
}  // namespace metal

}  // namespace crystal

#if CRYSTAL_USE_OPENGL
#include "crystal/opengl/context.hpp"
#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN
#include "crystal/vulkan/context.hpp"
#endif  // ^^^ CRYSTAL_USE_VULKAN

#if CRYSTAL_USE_METAL
#include "crystal/metal/context.hpp"
#endif  // ^^^ CRYSTAL_USE_METAL
