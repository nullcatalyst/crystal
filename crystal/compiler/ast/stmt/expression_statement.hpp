#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class ExpressionStatement : public Statement {
  util::memory::Ref<expr::Expression> expr_;

public:
  ExpressionStatement(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl::indent{opts.indent} << expr_->to_glsl(opts)
          << (opts.pretty ? ";\n" : ";");
    }};
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::metal::indent{opts.indent} << expr_->to_metal(opts) << ";\n";
    }};
  }
};

}  // namespace crystal::compiler::ast::stmt
