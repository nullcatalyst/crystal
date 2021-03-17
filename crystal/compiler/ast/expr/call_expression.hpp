#pragma once

#include <vector>

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/memory/ref_count.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast::expr {

class CallExpression : public Expression {
  util::memory::Ref<expr::Expression>              function_;
  std::vector<util::memory::Ref<expr::Expression>> arguments_;

public:
  CallExpression(util::memory::Ref<expr::Expression> function) : function_(function) {}
  CallExpression(util::memory::Ref<expr::Expression>              function,
                 std::vector<util::memory::Ref<expr::Expression>> arguments)
      : function_(function), arguments_(arguments) {}

  virtual ~CallExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override;

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override;
};

}  // namespace crystal::compiler::ast::expr
