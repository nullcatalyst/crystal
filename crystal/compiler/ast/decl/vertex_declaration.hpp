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

enum class VertexInputType {
  Undefined = 0,
  Vertex,
  Instanced,
  Uniform,
};

struct VertexInput {
  std::string                   name;
  util::memory::Ref<type::Type> type;
  VertexInputType               input_type;
  int32_t                       index;

  VertexInput(std::string_view name, util::memory::Ref<type::Type> type, VertexInputType input_type,
              int32_t index)
      : name(name), type(type), input_type(input_type), index(index) {}
};

class VertexDeclaration : public Declaration {
  std::string                                     name_;
  util::memory::Ref<type::Type>                   return_type_;
  std::vector<VertexInput>                        inputs_;
  std::vector<util::memory::Ref<stmt::Statement>> implementation_;

public:
  VertexDeclaration(std::string_view name, util::memory::Ref<type::Type> return_type,
                    std::vector<decl::VertexInput>&&                  inputs,
                    std::vector<util::memory::Ref<stmt::Statement>>&& implementation)
      : name_(name),
        return_type_(return_type),
        inputs_(std::move(inputs)),
        implementation_(std::move(implementation)) {}

  virtual ~VertexDeclaration() = default;

  [[nodiscard]] virtual const std::string&    name() const override { return name_; }
  [[nodiscard]] util::memory::Ref<type::Type> return_type() const { return return_type_; }

  void to_glsl(std::ostream& out, Module& mod) const;
  void to_pretty_glsl(std::ostream& out, Module& mod) const;
};

}  // namespace crystal::compiler::ast::decl
