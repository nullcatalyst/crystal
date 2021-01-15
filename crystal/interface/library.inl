// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

namespace crystal::CRYSTAL_IMPL {

class Context;
class Shader;
class Pipeline;

class Library {
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

public:
  Library() = delete;

  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;

  Library(Library&& other) = default;
  Library& operator=(Library&& other) = default;

  ~Library() = default;

private:
  friend class ::crystal::CRYSTAL_IMPL::Context;
  friend class ::crystal::CRYSTAL_IMPL::Shader;
  friend class ::crystal::CRYSTAL_IMPL::Pipeline;

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
