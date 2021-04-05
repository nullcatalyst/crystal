#include "crystal/vulkan/library.hpp"

#include <fstream>

#include "util/fs/file.hpp"

namespace crystal::vulkan {

Library::Library(Library&& other)
    : device_(other.device_),
      shader_module_(other.shader_module_),
      lib_pb_(std::move(other.lib_pb_)) {
  other.device_        = VK_NULL_HANDLE;
  other.shader_module_ = VK_NULL_HANDLE;
}

Library& Library::operator=(Library&& other) {
  destroy();

  device_        = other.device_;
  shader_module_ = other.shader_module_;
  lib_pb_        = std::move(other.lib_pb_);

  other.device_        = VK_NULL_HANDLE;
  other.shader_module_ = VK_NULL_HANDLE;

  return *this;
}

Library::~Library() { destroy(); }

void Library::destroy() noexcept {
  if (device_ == VK_NULL_HANDLE) {
    return;
  }

  vkDestroyShaderModule(device_, shader_module_, nullptr);

  device_        = VK_NULL_HANDLE;
  shader_module_ = VK_NULL_HANDLE;
}

Library::Library(const VkDevice device, const std::string_view file_path) : device_(device) {
  std::ifstream input_file(std::string(file_path), std::ios::in | std::ios::binary);
  if (!input_file) {
    util::msg::fatal("crystal library file [", file_path, "] not found");
  }
  if (!lib_pb_.ParseFromIstream(&input_file)) {
    util::msg::fatal("parsing crystal library from file [", file_path, "]");
  }

  const std::string& spv = lib_pb_.vulkan().library();

  const VkShaderModuleCreateInfo create_info = {
      /* .sType = */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      /* .pNext    = */ nullptr,
      /* .flags    = */ 0,
      /* .codeSize = */ spv.size(),
      /* .pCode    = */ reinterpret_cast<const uint32_t*>(spv.data()),
  };

  VK_ASSERT(vkCreateShaderModule(device_, &create_info, nullptr, &shader_module_),
            "creating shader module");
}

}  // namespace crystal::vulkan
