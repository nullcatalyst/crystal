#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "crystal/compiler/ast/decl/declaration.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast {

class Module;

}  // namespace crystal::compiler::ast

namespace crystal::compiler::ast::decl {

enum class FragmentInputType {
  Undefined = 0,
  Varying,
  Uniform,
  Texture,
};

struct FragmentInput {
  std::string                   name;
  util::memory::Ref<type::Type> type;
  FragmentInputType             input_type;
  int32_t                       index;

  FragmentInput(std::string_view name, util::memory::Ref<type::Type> type,
                FragmentInputType input_type, int32_t index)
      : name(name), type(type), input_type(input_type), index(index) {}
};

class FragmentDeclaration : public Declaration {
  util::memory::Ref<type::Type>                   return_type_;
  std::vector<FragmentInput>                      inputs_;
  std::vector<util::memory::Ref<stmt::Statement>> implementation_;

public:
  FragmentDeclaration(util::memory::Ref<type::Type>                     return_type,
                      std::vector<decl::FragmentInput>&&                inputs,
                      std::vector<util::memory::Ref<stmt::Statement>>&& implementation)
      : Declaration(""),
        return_type_(return_type),
        inputs_(std::move(inputs)),
        implementation_(std::move(implementation)) {}

  virtual ~FragmentDeclaration() = default;

  [[nodiscard]] util::memory::Ref<type::Type>    return_type() const { return return_type_; }
  [[nodiscard]] const std::vector<FragmentInput> inputs() const { return inputs_; }

  void set_name(const std::string_view name) { name_ = name; }

  void add_uniform(util::memory::Ref<type::Type> type, std::string name, int32_t index) {
    inputs_.emplace_back(name, type, FragmentInputType::Uniform, index);
  }

  void to_glsl(std::ostream& out, const Module& mod) const;
  void to_pretty_glsl(std::ostream& out, const Module& mod) const;
  void to_metal(std::ostream& out, const Module& mod) const;
};

}  // namespace crystal::compiler::ast::decl
