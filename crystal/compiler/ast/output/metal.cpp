#include "crystal/compiler/ast/output/metal.hpp"

#include "crystal/compiler/ast/decl/pipeline_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"

namespace crystal::compiler::ast::output::metal {

uint32_t uniform_binding(const decl::PipelineDeclaration& pipeline, uint32_t binding) {
  uint32_t max_buffer_index       = 0;
  uint32_t uniform_index          = 0;
  uint32_t matching_uniform_index = 0;
  for (const auto& input : pipeline.vertex_function()->inputs()) {
    if (input.input_type == decl::VertexInputType::Uniform) {
      if (input.index == binding) {
        matching_uniform_index = uniform_index;
      }
      ++uniform_index;
    }

    if (input.input_type == decl::VertexInputType::Vertex ||
        input.input_type == decl::VertexInputType::Instanced) {
      if (input.index >= max_buffer_index) {
        max_buffer_index = input.index + 1;
      }
    }
  }

  return max_buffer_index + matching_uniform_index;
}

}  // namespace crystal::compiler::ast::output::metal
