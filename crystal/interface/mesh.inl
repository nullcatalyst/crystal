// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

namespace crystal::CRYSTAL_IMPL {

class Context;
class CommandBuffer;
class IndexBuffer;
class VertexBuffer;

class Mesh {
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

public:
  Mesh() = delete;

  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  ~Mesh();

private:
  friend class ::crystal::CRYSTAL_IMPL::Context;
  friend class ::crystal::CRYSTAL_IMPL::CommandBuffer;

#ifdef CRYSTAL_IMPL_CTOR
  CRYSTAL_IMPL_CTOR
#endif  // CRYSTAL_IMPL_CTOR

#ifdef CRYSTAL_IMPL_METHODS
  CRYSTAL_IMPL_METHODS
#endif  // CRYSTAL_IMPL_METHODS
};

}  // namespace crystal::CRYSTAL_IMPL

#undef CRYSTAL_IMPL
#undef CRYSTAL_IMPL_PROPERTIES
#undef CRYSTAL_IMPL_CTOR
#undef CRYSTAL_IMPL_METHODS
