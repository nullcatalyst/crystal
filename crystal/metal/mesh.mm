#include "crystal/metal/mesh.hpp"

#include <algorithm>  // transform

#include "crystal/metal/context.hpp"
#include "crystal/metal/vertex_buffer.hpp"
#include "util/msg/msg.hpp"

namespace crystal::metal {

Mesh::Mesh(Mesh&& other)
    : binding_count_(other.binding_count_),
      bindings_(other.bindings_),
      index_buffer_(other.index_buffer_) {
  other.binding_count_ = 0;
  other.bindings_      = {};
  other.index_buffer_  = nullptr;
}

Mesh& Mesh::operator=(Mesh&& other) {
  destroy();

  binding_count_ = other.binding_count_;
  bindings_      = other.bindings_;
  index_buffer_  = other.index_buffer_;

  other.binding_count_ = 0;
  other.bindings_      = {};
  other.index_buffer_  = nullptr;

  return *this;
}

Mesh::~Mesh() { destroy(); }

void Mesh::destroy() noexcept {
  binding_count_ = 0;
  bindings_      = {};
  index_buffer_  = nullptr;
}

Mesh::Mesh(Context&                                                               ctx,
           const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings)
    : binding_count_(static_cast<uint32_t>(bindings.size())) {
  std::transform(bindings.begin(), bindings.end(), bindings_.begin(), [](auto binding) {
    const auto [index, vertex_buffer] = binding;
    if (vertex_buffer.buffer_ == nullptr) {
      util::msg::fatal("creating mesh from moved vertex buffer");
    }

    return Binding{
        /* .binding = */ index,
        /* .buffer  = */ vertex_buffer.buffer_,
    };
  });
}

Mesh::Mesh(Context&                                                               ctx,
           const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
           const IndexBuffer&                                                     index_buffer)
    : binding_count_(static_cast<uint32_t>(bindings.size())), index_buffer_(index_buffer.buffer_) {
  std::transform(bindings.begin(), bindings.end(), bindings_.begin(), [](auto binding) {
    const auto [index, vertex_buffer] = binding;
    if (vertex_buffer.buffer_ == nullptr) {
      util::msg::fatal("creating mesh from moved vertex buffer");
    }

    return Binding{
        /* .binding = */ index,
        /* .buffer  = */ vertex_buffer.buffer_,
    };
  });

  if (index_buffer_ == nullptr) {
    util::msg::fatal("creating mesh from moved index buffer");
  }
}

}  // namespace crystal::metal
