#pragma once

#include <cstdint>
#include <utility>

namespace util::unsafe {

/// DeferredCtor is designed to fix a problem with C++. C++ is very well intentioned in not allowing
/// the developer to access an uninitialized member variable of a class before it is initialized,
/// but sometimes deferring the initialization of a member to the body of the constructor can be
/// very useful for performance reasons. This is a very unsafe operation (think about what happens
/// if an exception is thrown in the containing class's constructor before this member is
/// initialized -> the member dtor will be called and it won't be a valid object).
///
/// MAKE SURE YOU KNOW WHAT YOU ARE DOING IF YOU PLAN ON USING THIS!!!
template <typename T>
class DeferredCtor {
  union {
    [[alignas(T)]] uint8_t storage_[sizeof(T)];
    T                      value_;
  };

public:
  constexpr DeferredCtor() {}

  ~DeferredCtor() { value_.~T(); }

  constexpr T*       operator->() noexcept { return &value_; }
  constexpr const T* operator->() const noexcept { return &value_; }

  constexpr operator T&() noexcept { return value_; }
  constexpr operator const T&() const noexcept { return value_; }

  template <typename... Args>
  void unsafe_call_ctor(Args&&... args) {
    new (&value_) T(std::forward<Args>(args)...);
  }
};

}  // namespace util::unsafe
