#include "crystal/vulkan/library.hpp"

#include "util/fs/file.hpp"

namespace crystal::vulkan {

Library::Library(const VkDevice device, const std::string& spv_path) {
  const auto contents = util::fs::read_file_binary(spv_path);
  // util::msg::debug("file contents size=", contents.size());
  const VkShaderModuleCreateInfo create_info = {
      /* .sType = */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      /* .pNext    = */ nullptr,
      /* .flags    = */ 0,
      /* .codeSize = */ contents.size(),
      /* .pCode    = */ reinterpret_cast<const uint32_t*>(contents.data()),
  };

  VK_ASSERT(vkCreateShaderModule(device, &create_info, nullptr, &shader_module_),
            "creating shader module");
}

}  // namespace crystal::vulkan
