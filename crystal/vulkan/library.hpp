#pragma once

#include <string>

#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class Shader;
class Pipeline;

class Library {
  VkDevice device_ = VK_NULL_HANDLE;
  VkShaderModule shader_module_ = VK_NULL_HANDLE;

public:
  constexpr Library() = default;

  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;

  Library(Library&& other);
  Library& operator=(Library&& other);

  ~Library();

  void destroy() noexcept;

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::Shader;
  friend class ::crystal::vulkan::Pipeline;

  Library(const VkDevice device, const std::string& spv_path);
};

}  // namespace crystal::vulkan
