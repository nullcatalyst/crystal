#pragma once

#include <array>
#include <initializer_list>
#include <tuple>

#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;
class IndexBuffer;
class VertexBuffer;

class Mesh {
  struct Binding {
    uint32_t index;
    VkBuffer buffer;
  };

  Context*               ctx_           = nullptr;
  uint32_t               binding_count_ = 0;
  std::array<Binding, 8> bindings_;
  VkBuffer               index_buffer_ = VK_NULL_HANDLE;

public:
  constexpr Mesh() = default;

  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  ~Mesh();

  void destroy() noexcept;

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;

  Mesh(Context&                                                               ctx,
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings);
  Mesh(Context&                                                               ctx,
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
       const IndexBuffer&                                                     index_buffer);
};

}  // namespace crystal::vulkan
