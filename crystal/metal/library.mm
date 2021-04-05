#include "crystal/metal/library.hpp"

#include <fstream>

#include "util/msg/msg.hpp"

namespace crystal::metal {

Library::Library(OBJC(MTLDevice) device, const std::string_view file_path) {
  std::ifstream input_file(std::string(file_path), std::ios::in | std::ios::binary);
  if (!input_file) {
    util::msg::fatal("crystal library file [", file_path, "] not found");
  }
  if (!lib_pb_.ParseFromIstream(&input_file)) {
    util::msg::fatal("parsing crystal library from file [", file_path, "]");
  }

  dispatch_data_t data =
      dispatch_data_create(lib_pb_.metal().library().c_str(), lib_pb_.metal().library().size(),
                           dispatch_get_main_queue(), DISPATCH_DATA_DESTRUCTOR_DEFAULT);

  NSError* err = nullptr;
  library_     = [device newLibraryWithData:(dispatch_data_t)data error:&err];
  if (err != nullptr) {
    util::msg::fatal("loading metal library: ", [[err description] UTF8String]);
  }
}

}  //
