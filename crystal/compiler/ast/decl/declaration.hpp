#pragma once

namespace crystal::compiler::ast::decl {

class Declaration {
public:
  virtual ~Declaration() = default;

  [[nodiscard]] virtual const std::string& name() const = 0;
};

}  // namespace crystal::compiler::ast::decl
