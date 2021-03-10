#pragma once

#include <string>
#include <string_view>

#include "crystal/compiler/ast/decl/declaration.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast {

class Module;

}  // namespace crystal::compiler::ast

namespace crystal::compiler::ast::decl {

class VertexDeclaration;
class FragmentDeclaration;

struct PipelineSettings {
  std::string vertex_name;
  std::string fragment_name;
};

class PipelineDeclaration : public Declaration {
  util::memory::Ref<VertexDeclaration>   vertex_function_;
  util::memory::Ref<FragmentDeclaration> fragment_function_;

public:
  PipelineDeclaration(std::string_view name, const Module& mod, const PipelineSettings& settings);

  virtual ~PipelineDeclaration() = default;

  void to_cpphdr(std::ostream& out, const Module& mod) const;
};

}  // namespace crystal::compiler::ast::decl
