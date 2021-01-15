// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

namespace crystal::CRYSTAL_IMPL {

class Context;
class Mesh;

class IndexBuffer {
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

public:
  IndexBuffer() = delete;

  IndexBuffer(const IndexBuffer&) = delete;
  IndexBuffer& operator=(const IndexBuffer&) = delete;

  IndexBuffer(IndexBuffer&& other);
  IndexBuffer& operator=(IndexBuffer&& other);

  ~IndexBuffer();

private:
  friend class ::crystal::CRYSTAL_IMPL::Context;
  friend class ::crystal::CRYSTAL_IMPL::Mesh;

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
