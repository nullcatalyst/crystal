#include "crystal/metal/library.hpp"

namespace crystal::metal {

Library::Library(OBJC(MTLDevice) device, const std::string_view path) {
  library_ = [device newLibraryWithFile:[NSString stringWithCString:path.data() length:path.size()]
                                  error:nullptr];
}

}  //
