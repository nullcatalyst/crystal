#include "crystal/vulkan/mesh.hpp"

#include <algorithm>

#include "crystal/vulkan/context.hpp"
#include "crystal/vulkan/index_buffer.hpp"
#include "crystal/vulkan/vertex_buffer.hpp"

namespace crystal::vulkan {

Mesh::Mesh(Mesh&& other)
    : ctx_(other.ctx_),
      binding_count_(other.binding_count_),
      bindings_(std::move(other.bindings_)),
      index_buffer_(other.index_buffer_) {
  other.ctx_           = nullptr;
  other.binding_count_ = 0;
  other.index_buffer_  = VK_NULL_HANDLE;
}

Mesh& Mesh::operator=(Mesh&& other) {
  for (uint32_t i = 0; i != binding_count_; ++i) {
    ctx_->release_buffer_(bindings_[i].buffer);
  }

  if (index_buffer_ != VK_NULL_HANDLE) {
    ctx_->release_buffer_(index_buffer_);
  }

  ctx_           = other.ctx_;
  binding_count_ = other.binding_count_;
  bindings_      = std::move(other.bindings_);
  index_buffer_  = other.index_buffer_;

  other.ctx_           = nullptr;
  other.binding_count_ = 0;
  other.index_buffer_  = VK_NULL_HANDLE;

  return *this;
}

Mesh::~Mesh() {
  for (uint32_t i = 0; i != binding_count_; ++i) {
    ctx_->release_buffer_(bindings_[i].buffer);
  }

  if (index_buffer_ != VK_NULL_HANDLE) {
    ctx_->release_buffer_(index_buffer_);
  }
}

Mesh::Mesh(Context&                                                               ctx,
           const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings)
    : ctx_(&ctx), index_buffer_(VK_NULL_HANDLE) {
  binding_count_ = bindings.size();
  std::transform(bindings.begin(), bindings.end(), bindings_.begin(), [](auto binding) {
    const auto [index, vertex_buffer] = binding;
    if (vertex_buffer.ctx_ == nullptr) {
      util::msg::fatal("creating mesh from moved vertex buffer");
    }

    vertex_buffer.ctx_->retain_buffer_(vertex_buffer.buffer_);
    return Binding{
        /* .binding = */ index,
        /* .buffer  = */ vertex_buffer.buffer_,
    };
  });
}

Mesh::Mesh(Context&                                                               ctx,
           const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
           const IndexBuffer&                                                     index_buffer)
    : ctx_(&ctx), index_buffer_(index_buffer.buffer_) {
  binding_count_ = bindings.size();
  std::transform(bindings.begin(), bindings.end(), bindings_.begin(), [ctx = ctx_](auto binding) {
    const auto [index, vertex_buffer] = binding;
    if (vertex_buffer.ctx_ == nullptr) {
      util::msg::fatal("creating mesh from moved vertex buffer");
    }

    ctx->retain_buffer_(vertex_buffer.buffer_);
    return Binding{
        /* .binding = */ index,
        /* .buffer  = */ vertex_buffer.buffer_,
    };
  });

  if (index_buffer.ctx_ == nullptr) {
    util::msg::fatal("creating mesh from moved index buffer");
  }
  ctx_->retain_buffer_(index_buffer.buffer_);
}

}  // namespace crystal::vulkan
