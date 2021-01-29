#include "crystal/opengl/mesh.hpp"

#include <algorithm>  // transform

#include "crystal/opengl/context.hpp"
#include "crystal/opengl/vertex_buffer.hpp"

namespace crystal::opengl {

Mesh::Vao::~Vao() { GL_ASSERT(glDeleteVertexArrays(1, &vao), "deleting vertex array"); }

Mesh::Mesh(Mesh&& other)
    : ctx_(other.ctx_),
      vaos_(std::move(other.vaos_)),
      bindings_(std::move(other.bindings_)),
      index_buffer_(other.index_buffer_) {
  other.ctx_          = nullptr;
  other.index_buffer_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
  destroy();

  ctx_          = other.ctx_;
  vaos_         = std::move(other.vaos_);
  bindings_     = std::move(other.bindings_);
  index_buffer_ = other.index_buffer_;

  other.ctx_          = nullptr;
  other.index_buffer_ = 0;

  return *this;
}

Mesh::~Mesh() { destroy(); }

void Mesh::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  for (uint32_t i = 0; i != bindings_.size(); ++i) {
    ctx_->release_buffer_(bindings_[i].vertex_buffer);
  }

  if (index_buffer_ != 0) {
    ctx_->release_buffer_(index_buffer_);
  }

  ctx_ = nullptr;
  vaos_.resize(0);
  bindings_.resize(0);
  index_buffer_ = 0;
}

Mesh::Mesh(Context&                                                               ctx,
           const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings)
    : ctx_(&ctx), index_buffer_(0) {
  bindings_.resize(bindings.size());
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
    : Mesh(ctx, bindings) {
  index_buffer_ = index_buffer.buffer_;
  ctx_->retain_buffer_(index_buffer_);
}

}  // namespace crystal::opengl
