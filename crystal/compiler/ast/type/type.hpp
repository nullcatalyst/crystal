#pragma once

#include <string>

namespace crystal::compiler::ast::type {

class Type {
  std::string name_;
  bool        builtin_;

public:
  Type(std::string_view name, bool builtin) : name_(name), builtin_(builtin) {}

  virtual ~Type() = default;

  [[nodiscard]] bool               builtin() const { return builtin_; }
  [[nodiscard]] const std::string& name() const { return name_; }
};

}  // namespace crystal::compiler::ast::type
