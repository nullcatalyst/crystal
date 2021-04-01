#include "crystal/compiler/ast/expr/call_expression.hpp"

#include "crystal/compiler/ast/expr/identifier_expression.hpp"
#include "crystal/compiler/ast/module.hpp"

namespace crystal::compiler::ast::expr {

output::PrintLambda CallExpression::to_glsl(const output::glsl::Options opts) const {
  // TODO: Typecheck to determine that the callee is a texture.
  if (expr_ != nullptr && name_ == "sample") {
    return output::PrintLambda{[=](std::ostream& out) {
      out << "texture(" << expr_->to_glsl(opts);
      for (const auto& arg : arguments_) {
        out << ", " << arg->to_glsl(opts);
      }
      out << ")";
    }};
  }

  if (expr_ != nullptr && name_ == "sampleDepth") {
    return output::PrintLambda{[=](std::ostream& out) {
      out << "(texture(" << expr_->to_glsl(opts);
      for (const auto& arg : arguments_) {
        out << ", " << arg->to_glsl(opts);
      }
      out << ").x * 2.0 - 1.0)";
    }};
  }

  const auto type = opts.mod.find_type(name_);
  if (type.has_value()) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << type.value()->glsl_name() << "(";
      bool first = true;
      for (const auto& arg : arguments_) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        out << arg->to_glsl(opts);
      }
      out << ")";
    }};
  }

  return output::PrintLambda{[=](std::ostream& out) {
    out << name_ << "(";
    bool first = true;
    for (const auto& arg : arguments_) {
      if (first) {
        first = false;
      } else {
        out << ", ";
      }
      out << arg->to_glsl(opts);
    }
    out << ")";
  }};
}

output::PrintLambda CallExpression::to_metal(const output::metal::Options opts) const {
  // TODO: Typecheck to determine that the callee is a texture.
  if (expr_ != nullptr && name_ == "sample") {
    return output::PrintLambda{[=](std::ostream& out) {
      out << expr_->to_metal(opts) << ".sample(" << expr_->to_metal(opts) << "_sampler";
      bool first = true;
      for (const auto& arg : arguments_) {
        if (first) {
          first = false;
          out << ", float2(" << arg->to_metal(opts) << ".x, 1.0 - " << arg->to_metal(opts) << ".y)";
        } else {
          out << ", " << arg->to_metal(opts);
        }
      }
      out << ")";
    }};
  }

  if (expr_ != nullptr && name_ == "sampleDepth") {
    return output::PrintLambda{[=](std::ostream& out) {
      out << expr_->to_metal(opts) << ".sample(" << expr_->to_metal(opts) << "_sampler";
      bool first = true;
      for (const auto& arg : arguments_) {
        if (first) {
          first = false;
          out << ", float2(" << arg->to_metal(opts) << ".x, 1.0 - " << arg->to_metal(opts) << ".y)";
        } else {
          out << ", " << arg->to_metal(opts);
        }
      }
      out << ").x";
    }};
  }

  const auto type = opts.mod.find_type(name_);
  if (type.has_value()) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << type.value()->metal_name() << "(";
      bool first = true;
      for (const auto& arg : arguments_) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        out << arg->to_metal(opts);
      }
      out << ")";
    }};
  }

  return output::PrintLambda{[=](std::ostream& out) {
    out << name_ << "(";
    bool first = true;
    for (const auto& arg : arguments_) {
      if (first) {
        first = false;
      } else {
        out << ", ";
      }
      out << arg->to_metal(opts);
    }
    out << ")";
  }};
}

}  // namespace crystal::compiler::ast::expr
