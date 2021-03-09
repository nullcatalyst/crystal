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

enum class VertexInputBufferType {
  Undefined = 0,
  Vertex,
  Instanced,
  Uniform,
};

struct VertexInputBuffer {
  std::string                   name;
  util::memory::Ref<type::Type> type;
  VertexInputBufferType         buffer_type;
  int32_t                       index;

  VertexInputBuffer(std::string_view name, util::memory::Ref<type::Type> type,
                    VertexInputBufferType buffer_type, int32_t index)
      : name(name), type(type), buffer_type(buffer_type), index(index) {}
};

class VertexDeclaration : public Declaration {
  std::string                                     name_;
  util::memory::Ref<type::Type>                   return_type_;
  std::vector<VertexInputBuffer>                  input_buffers_;
  std::vector<util::memory::Ref<stmt::Statement>> implementation_;

public:
  VertexDeclaration(std::string_view name, util::memory::Ref<type::Type> return_type,
                    std::vector<decl::VertexInputBuffer>&&            input_buffers,
                    std::vector<util::memory::Ref<stmt::Statement>>&& implementation)
      : name_(name),
        return_type_(return_type),
        input_buffers_(std::move(input_buffers)),
        implementation_(std::move(implementation)) {}

  virtual ~VertexDeclaration() = default;

  [[nodiscard]] virtual const std::string&    name() const override { return name_; }
  [[nodiscard]] util::memory::Ref<type::Type> return_type() const { return return_type_; }

  void to_glsl(std::ostream& out, Module& mod);
};

}  // namespace crystal::compiler::ast::decl
