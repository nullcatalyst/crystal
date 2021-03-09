#pragma once

#include <string>

#include "crystal/compiler/ast/decl/declaration.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::decl {

class StructDeclaration : public Declaration {
  util::memory::Ref<type::Type> type_;

public:
  StructDeclaration(util::memory::Ref<type::Type> type) : type_(type) {}

  virtual ~StructDeclaration() = default;

  [[nodiscard]] virtual const std::string& name() const { return type_->name(); }
};

}  // namespace crystal::compiler::ast::decl
