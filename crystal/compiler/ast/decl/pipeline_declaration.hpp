#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "crystal/common/proto/proto.hpp"
#include "crystal/compiler/ast/decl/declaration.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast {

class Module;

}  // namespace crystal::compiler::ast

namespace crystal::compiler::ast::decl {

class VertexDeclaration;
class FragmentDeclaration;

struct PipelineSettings {
  util::memory::Ref<VertexDeclaration>           vertex_function;
  util::memory::Ref<FragmentDeclaration>         fragment_function;
  std::vector<std::tuple<uint32_t, std::string>> uniforms;
};

class PipelineDeclaration : public Declaration {
  util::memory::Ref<VertexDeclaration>   vertex_function_;
  util::memory::Ref<FragmentDeclaration> fragment_function_;

public:
  PipelineDeclaration(std::string_view name, const PipelineSettings& settings);

  virtual ~PipelineDeclaration() = default;

  void to_cpphdr(std::ostream& out, const Module& mod) const;
  void to_crystallib(crystal::common::proto::Pipeline& pipeline_pb, const Module& mod) const;
};

}  // namespace crystal::compiler::ast::decl
