#pragma once

#include <string>
#include <string_view>

#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
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

  virtual output::PrintLambda to_glsl(const decl::VertexDeclaration& vertex) const override {
    return to_glsl();
  }

  virtual output::PrintLambda to_glsl(const decl::FragmentDeclaration& fragment) const override {
    return to_glsl();
  }

  virtual output::PrintLambda to_pretty_glsl(const decl::VertexDeclaration& vertex,
                                             uint32_t                       indent) const override {
    return to_pretty_glsl(indent);
  }

  virtual output::PrintLambda to_pretty_glsl(const decl::FragmentDeclaration& fragment,
                                             uint32_t indent) const override {
    return to_pretty_glsl(indent);
  }

private:
  output::PrintLambda to_glsl() const {
    return output::PrintLambda{[=](std::ostream& out) {
      out << type_->name() << " " << output::glsl_mangle_name{name_} << ";";
    }};
  }

  output::PrintLambda to_pretty_glsl(uint32_t indent) const {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl_indent{indent} << type_->name() << " " << output::glsl_mangle_name{name_}
          << ";\n";
    }};
  }
};

}  // namespace crystal::compiler::ast::stmt
