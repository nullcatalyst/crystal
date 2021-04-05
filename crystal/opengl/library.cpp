#include "crystal/opengl/library.hpp"

#include <fstream>

#include "util/msg/msg.hpp"

namespace crystal::opengl {

Library::Library(const std::string_view file_path) {
  std::ifstream input_file(std::string(file_path), std::ios::in | std::ios::binary);
  if (!input_file) {
    util::msg::fatal("crystal library file [", file_path, "] not found");
  }
  if (!lib_pb_.ParseFromIstream(&input_file)) {
    util::msg::fatal("parsing crystal library from file [", file_path, "]");
  }
}

}  // namespace crystal::opengl
