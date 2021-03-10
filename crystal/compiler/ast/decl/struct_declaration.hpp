#pragma once

#include <string>

#include "crystal/compiler/ast/decl/declaration.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::decl {

class StructDeclaration : public Declaration {
  util::memory::Ref<type::Type> type_;

public:
  StructDeclaration(util::memory::Ref<type::Type> type) : Declaration(type->name()), type_(type) {}

  virtual ~StructDeclaration() = default;
};

}  // namespace crystal::compiler::ast::decl
