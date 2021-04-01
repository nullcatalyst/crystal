#pragma once

#include <CoreFoundation/CFBase.h>  // CFRetain

#define OBJC_NAME($name) ObjC_##$name##_
#define OBJC($type) OBJC_NAME($type)

#ifdef __OBJC__

#include <AppKit/NSView.h>
#include <AppKit/NSWindow.h>
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>
#include <QuartzCore/CAMetalLayer.h>

#define OBJC_PRE_($name, $type)
#define OBJC_IMPL_($name, $type)                                       \
  constexpr OBJC_NAME($name)($type raw) : raw_((__bridge void*)raw) {} \
  constexpr OBJC_NAME($name)& operator=($type raw) noexcept {          \
    release_();                                                        \
    raw_ = (__bridge void*)raw;                                        \
    retain_();                                                         \
    return *this;                                                      \
  }                                                                    \
                                                                       \
  [[nodiscard]] operator $type() const noexcept { return (__bridge $type)raw_; }

#else  // ^^^ defined(__OBJC__) / !defined(__OBJC__) vvv

using MTLPixelFormat = unsigned long;
using MTLCullMode    = unsigned long;
using MTLWinding     = unsigned long;

#define OBJC($type) OBJC_NAME($type)

#define OBJC_PRE_($name, $type) using $name = struct $name;
#define OBJC_IMPL_($name, $type)

#endif  // ^^^ !defined(__OBJC__)

#define DEFINE_OBJC($name, $type)                                                                \
  OBJC_PRE_($name, $type)                                                                        \
  class OBJC_NAME($name) {                                                                       \
    void* raw_;                                                                                  \
                                                                                                 \
  public:                                                                                        \
    constexpr OBJC_NAME($name)() : raw_(nullptr) {}                                              \
                                                                                                 \
    constexpr OBJC_NAME($name)(const std::nullptr_t) : raw_(nullptr) {}                          \
    constexpr OBJC_NAME($name) & operator=(const std::nullptr_t) noexcept {                      \
      release_();                                                                                \
      return *this;                                                                              \
    }                                                                                            \
                                                                                                 \
    constexpr OBJC_NAME($name)(void* raw) : raw_(raw) {}                                         \
    constexpr OBJC_NAME($name) & operator=(void* raw) noexcept {                                 \
      release_();                                                                                \
      raw_ = raw;                                                                                \
      retain_();                                                                                 \
      return *this;                                                                              \
    }                                                                                            \
                                                                                                 \
    constexpr OBJC_NAME($name)(const OBJC_NAME($name) & other) : raw_(other.raw_) { retain_(); } \
    constexpr OBJC_NAME($name) & operator=(const OBJC_NAME($name) & other) noexcept {            \
      release_();                                                                                \
      raw_ = other.raw_;                                                                         \
      retain_();                                                                                 \
      return *this;                                                                              \
    }                                                                                            \
                                                                                                 \
    constexpr OBJC_NAME($name)(OBJC_NAME($name) && other) : raw_(other.raw_) {                   \
      other.raw_ = nullptr;                                                                      \
    }                                                                                            \
    constexpr OBJC_NAME($name) & operator=(OBJC_NAME($name) && other) noexcept {                 \
      raw_       = other.raw_;                                                                   \
      other.raw_ = nullptr;                                                                      \
      return *this;                                                                              \
    }                                                                                            \
                                                                                                 \
    ~OBJC_NAME($name)() { release_(); }                                                          \
                                                                                                 \
    [[nodiscard]] operator void*() const noexcept { return raw_; }                               \
                                                                                                 \
    [[nodiscard]] bool operator==(std::nullptr_t) const noexcept { return raw_ == nullptr; }     \
    [[nodiscard]] bool operator!=(std::nullptr_t) const noexcept { return raw_ != nullptr; }     \
                                                                                                 \
    OBJC_IMPL_($name, $type)                                                                     \
                                                                                                 \
  private:                                                                                       \
    void retain_() noexcept {                                                                    \
      if (raw_ != nullptr) {                                                                     \
        CFRetain(raw_);                                                                          \
      }                                                                                          \
    }                                                                                            \
                                                                                                 \
    void release_() noexcept {                                                                   \
      if (raw_ != nullptr) {                                                                     \
        CFRelease(raw_);                                                                         \
        raw_ = nullptr;                                                                          \
      }                                                                                          \
    }                                                                                            \
  };

DEFINE_OBJC(CAMetalDrawable, id<CAMetalDrawable>)
DEFINE_OBJC(CAMetalLayer, CAMetalLayer*)
DEFINE_OBJC(MTLBuffer, id<MTLBuffer>)
DEFINE_OBJC(MTLCommandBuffer, id<MTLCommandBuffer>)
DEFINE_OBJC(MTLCommandQueue, id<MTLCommandQueue>)
DEFINE_OBJC(MTLDepthStencilState, id<MTLDepthStencilState>)
DEFINE_OBJC(MTLDevice, id<MTLDevice>)
DEFINE_OBJC(MTLFunction, id<MTLFunction>)
DEFINE_OBJC(MTLLibrary, id<MTLLibrary>)
DEFINE_OBJC(MTLRenderCommandEncoder, id<MTLRenderCommandEncoder>)
DEFINE_OBJC(MTLRenderPassDescriptor, MTLRenderPassDescriptor*)
DEFINE_OBJC(MTLRenderPipelineState, id<MTLRenderPipelineState>)
DEFINE_OBJC(MTLSamplerState, id<MTLSamplerState>)
DEFINE_OBJC(MTLTexture, id<MTLTexture>)
DEFINE_OBJC(MTLTextureDescriptor, MTLTextureDescriptor*)
DEFINE_OBJC(MTLVertexDescriptor, MTLVertexDescriptor*)
