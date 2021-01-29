#include "crystal/vulkan/library.hpp"

#include "util/fs/file.hpp"

namespace crystal::vulkan {

Library::Library(Library&& other) : device_(other.device_), shader_module_(other.shader_module_) {
  other.device_        = VK_NULL_HANDLE;
  other.shader_module_ = VK_NULL_HANDLE;
}

Library& Library::operator=(Library&& other) {
  destroy();

  other.device_        = device_;
  other.shader_module_ = shader_module_;

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

Library::Library(const VkDevice device, const std::string& spv_path) : device_(device) {
  const auto contents = util::fs::read_file_binary(spv_path);
  // util::msg::debug("file contents size=", contents.size());
  const VkShaderModuleCreateInfo create_info = {
      /* .sType = */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      /* .pNext    = */ nullptr,
      /* .flags    = */ 0,
      /* .codeSize = */ contents.size(),
      /* .pCode    = */ reinterpret_cast<const uint32_t*>(contents.data()),
  };

  VK_ASSERT(vkCreateShaderModule(device_, &create_info, nullptr, &shader_module_),
            "creating shader module");
}

}  // namespace crystal::vulkan
