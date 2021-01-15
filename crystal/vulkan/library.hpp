#pragma once

#include <string>

#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES VkShaderModule shader_module_;
#define CRYSTAL_IMPL_CTOR Library(const VkDevice device, const std::string& spv_path);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/library.inl"
