#pragma once

#include <string>
#include <string_view>

#include "crystal/compiler/ast/stmt/statement.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class VariableStatement : public Statement {
  std::string                   name_;
  util::memory::Ref<type::Type> type_;

public:
  VariableStatement(std::string_view name, util::memory::Ref<type::Type> type)
      : name_(name), type_(type) {}

  virtual void to_glsl(std::ostream& out, decl::VertexDeclaration& vertex, int indent) override {
    indent_glsl(out, indent);
    out << type_->name() << " _" << name_ << ";\n";
  }
};

}  // namespace crystal::compiler::ast::stmt
