#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

enum class AssignmentOp {
  Undefined = 0,
  Set,
  Add,
  Sub,
  Mul,
  Div,
};

class AssignmentStatement : public Statement {
  util::memory::Ref<expr::Expression> var_;
  util::memory::Ref<expr::Expression> value_;
  AssignmentOp                        op_;

public:
  AssignmentStatement(util::memory::Ref<expr::Expression> var,
                      util::memory::Ref<expr::Expression> value, AssignmentOp op)
      : var_(var), value_(value), op_(op) {}

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override;

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override;
};

}  // namespace crystal::compiler::ast::stmt
