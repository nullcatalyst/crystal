#pragma once

#include <string>
#include <string_view>

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class VariableStatement : public Statement {
  std::string                         name_;
  util::memory::Ref<type::Type>       type_;
  util::memory::Ref<expr::Expression> expr_;

public:
  VariableStatement(std::string_view name, util::memory::Ref<type::Type> type)
      : name_(name), type_(type) {}
  VariableStatement(std::string_view name, util::memory::Ref<type::Type> type,
                    util::memory::Ref<expr::Expression> expr)
      : name_(name), type_(type), expr_(expr) {}

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override;

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override;
};

}  // namespace crystal::compiler::ast::stmt
