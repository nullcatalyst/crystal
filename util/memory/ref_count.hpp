#pragma once

#include <utility>

namespace util::memory {

template <typename T, typename Deleter = void>
struct RefCount {
private:
  uint32_t                      ref_count_;
  T                             value_;
  [[no_unique_address]] Deleter deleter_;

public:
  template <typename... Args>
  RefCount(Args&&... args) : ref_count_(1), value_(std::forward<Args>(args)...) {}

  constexpr    operator T&() { return value_; }
  constexpr T* operator->() { return &value_; }

  void retain() { ++ref_count_; }

  bool release() {
    --ref_count_;
    if (ref_count_ == 0) {
      deleter_(value_);
      return true;
    }
    return false;
  }
};  // namespace util::memory

template <typename T>
struct RefCount<T, void> {
private:
  uint32_t ref_count_;
  T        value_;

public:
  template <typename... Args>
  RefCount(Args&&... args) : ref_count_(1), value_(std::forward<Args>(args)...) {}

  constexpr    operator T&() { return value_; }
  constexpr T* operator->() { return &value_; }

  void retain() { ++ref_count_; }

  bool release() {
    --ref_count_;
    return ref_count_ == 0;
  }
};

}  // namespace util::memory
