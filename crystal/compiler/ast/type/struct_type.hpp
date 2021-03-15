#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::type {

struct StructProperty {
  std::string             name;
  util::memory::Ref<Type> type;
  int32_t                 index;

  StructProperty(std::string_view name, util::memory::Ref<Type> type)
      : name(name), type(type), index(-1) {}

  StructProperty(std::string_view name, util::memory::Ref<Type> type, int32_t index)
      : name(name), type(type), index(index) {}
};

class StructType : public Type {
  std::vector<StructProperty> properties_;

public:
  StructType(std::string_view name, bool builtin) : Type(name, builtin) {}

  StructType(std::string_view name, bool builtin, std::vector<StructProperty>&& properties)
      : Type(name, builtin), properties_(std::move(properties)) {}

  virtual ~StructType() = default;

  [[nodiscard]] const std::vector<StructProperty>& properties() const { return properties_; }
};

}  // namespace crystal::compiler::ast::type
