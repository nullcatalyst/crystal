#pragma once

#include <array>
#include <initializer_list>
#include <tuple>

#include "absl/container/inlined_vector.h"
#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                    \
  struct Binding {                                 \
    uint32_t index;                                \
    VkBuffer buffer;                               \
  };                                               \
                                                   \
  Context*               ctx_           = nullptr; \
  uint32_t               binding_count_ = 0;       \
  std::array<Binding, 8> bindings_;                \
  VkBuffer               index_buffer_ = VK_NULL_HANDLE;
#define CRYSTAL_IMPL_CTOR                                                                \
  Mesh(Context&                                                               ctx,       \
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings); \
  Mesh(Context&                                                               ctx,       \
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,  \
       const IndexBuffer&                                                     index_buffer);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/mesh.inl"
