#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/memory/ref_count.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast::expr {

class CallExpression : public Expression {
  util::memory::Ref<expr::Expression>              expr_;
  std::string                                      name_;
  std::vector<util::memory::Ref<expr::Expression>> arguments_;

public:
  CallExpression(const std::string_view name) : name_(name) {}
  CallExpression(const std::string_view                           name,
                 std::vector<util::memory::Ref<expr::Expression>> arguments)
      : name_(name), arguments_(arguments) {}

  CallExpression(util::memory::Ref<expr::Expression> expr, const std::string_view name)
      : expr_(expr), name_(name) {}
  CallExpression(util::memory::Ref<expr::Expression> expr, const std::string_view name,
                 std::vector<util::memory::Ref<expr::Expression>> arguments)
      : expr_(expr), name_(name), arguments_(arguments) {}

  virtual ~CallExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override;

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override;
};

}  // namespace crystal::compiler::ast::expr
