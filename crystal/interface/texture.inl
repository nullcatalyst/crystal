// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

#include "crystal/common/texture_desc.hpp"
#include "util/unsafe/deferred_ctor.hpp"

namespace crystal::CRYSTAL_IMPL {

class Context;
class RenderPass;

class Texture {
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

public:
  Texture() = delete;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other);
  Texture& operator=(Texture&& other);

  ~Texture();

private:
  friend class ::crystal::CRYSTAL_IMPL::Context;
  friend class ::crystal::CRYSTAL_IMPL::RenderPass;
  friend class ::util::unsafe::DeferredCtor<Texture>;

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
