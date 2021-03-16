#pragma once

#include <string>

namespace crystal::compiler::ast::type {

class Type {
  std::string name_;
  std::string glsl_name_;
  std::string metal_name_;
  bool        builtin_;

public:
  Type(std::string_view name, bool builtin) : name_(name), builtin_(builtin) {}

  virtual ~Type() = default;

  void set_glsl_name(const std::string_view name) { glsl_name_ = name; }
  void set_metal_name(const std::string_view name) { metal_name_ = name; }

  [[nodiscard]] bool               builtin() const { return builtin_; }
  [[nodiscard]] const std::string& name() const { return name_; }
  [[nodiscard]] const std::string& glsl_name() const {
    return glsl_name_.size() > 0 ? glsl_name_ : name_;
  }
  [[nodiscard]] const std::string& metal_name() const {
    return metal_name_.size() > 0 ? metal_name_ : name_;
  }
};

}  // namespace crystal::compiler::ast::type
