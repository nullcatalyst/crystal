#pragma once

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif  // NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR

#else  // #ifdef _WIN32

#include <vulkan/vulkan.h>

#endif  // #ifdef _WIN32

#ifdef _MSVC_LANG

/*
In every place where you want to use Vulkan Memory Allocator, define appropriate
macros if you want to configure the library and then include its header to
include all public interface declarations. Example:
*/

//#define VMA_HEAVY_ASSERT(expr) assert(expr)
//#define VMA_USE_STL_CONTAINERS 1
//#define VMA_DEDICATED_ALLOCATION 0
//#define VMA_DEBUG_MARGIN 16
//#define VMA_DEBUG_DETECT_CORRUPTION 1
//#define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
//#define VMA_RECORDING_ENABLED 1
//#define VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY 256
//#define VMA_USE_STL_SHARED_MUTEX 0
//#define VMA_DEBUG_GLOBAL_MUTEX 1
//#define VMA_MEMORY_BUDGET 0

#define VMA_VULKAN_VERSION 1001000  // Vulkan 1.1
//#define VMA_VULKAN_VERSION 1000000 // Vulkan 1.0

/*
#define VMA_DEBUG_LOG(format, ...) do { \
        printf(format, __VA_ARGS__); \
        printf("\n"); \
    } while(false)
*/

#pragma warning(push, 4)
#pragma warning(disable : 4127)  // conditional expression is constant
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4189)  // local variable is initialized but not referenced
#pragma warning(disable : 4324)  // structure was padded due to alignment specifier

#endif  // #ifdef _MSVC_LANG

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored \
    "-Wtautological-compare"  // comparison of unsigned expression < 0 is always false
#pragma clang diagnostic ignored "-Wunused-private-field"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

#include "vk_mem_alloc.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSVC_LANG
#pragma warning(pop)
#endif

#include <cstdio>
#include <cstdlib>

#include "util/msg/msg.hpp"

#ifdef CRYSTAL_RELEASE

#define VK_ASSERT($call, $msg, ...) ($call)

#else

#define VK_ASSERT($call, $msg, ...)                                                              \
  do {                                                                                           \
    VkResult $result = ($call);                                                                  \
    if ($result != VK_SUCCESS) {                                                                 \
      const char* $err = vkResultToString($result);                                              \
      util::msg::fatal(stderr, __FILE__, ":", __LINE__, ": (", $err, ") ", $msg, ##__VA_ARGS__); \
    }                                                                                            \
  } while (0)

#endif  // CRYSTAL_RELEASE

inline const char* vkResultToString(VkResult result) {
  const char* err_msg = "<unknown>";
  switch (result) {
#define X(x)      \
  case x:         \
    err_msg = #x; \
    break;
#include "crystal/vulkan/vk_error.inl"
#undef X
    default:
      break;
  }
  return err_msg;
}
