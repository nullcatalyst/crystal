#pragma once

#include <string_view>

#include "crystal/common/proto/proto.hpp"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class Shader;
class Pipeline;

class Library {
  VkDevice               device_        = VK_NULL_HANDLE;
  VkShaderModule         shader_module_ = VK_NULL_HANDLE;
  common::proto::Library lib_pb_;

public:
  Library() = default;

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

  Library(VkDevice device, const std::string_view file_path);
};

}  // namespace crystal::vulkan
