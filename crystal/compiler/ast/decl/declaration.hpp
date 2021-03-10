#pragma once

#include <string>
#include <string_view>

namespace crystal::compiler::ast::decl {

class Declaration {
  std::string name_;

public:
  Declaration(std::string_view name) : name_(name) {}

  virtual ~Declaration() = default;

  [[nodiscard]] const std::string& name() const { return name_; }
};

}  // namespace crystal::compiler::ast::decl
