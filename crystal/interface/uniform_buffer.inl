// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

namespace crystal::CRYSTAL_IMPL {

class Context;
class CommandBuffer;

class UniformBuffer {
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

public:
  UniformBuffer() = delete;

  UniformBuffer(const UniformBuffer&) = delete;
  UniformBuffer& operator=(const UniformBuffer&) = delete;

  UniformBuffer(UniformBuffer&& other);
  UniformBuffer& operator=(UniformBuffer&& other);

  ~UniformBuffer();

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
