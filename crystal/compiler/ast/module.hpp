#pragma once

#include <optional>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "crystal/compiler/ast/decl/fragment_declaration.hpp"
#include "crystal/compiler/ast/decl/pipeline_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast {

struct CppOutputOptions {
  bool cpp17;
};

struct MetalOutputOptions {};

struct CrystallibOutputOptions {
  bool opengl;
  bool vulkan;
  bool metal;

  // Path to the glslangValidator executable that will be used to convert glsl to spv.
  std::string_view glslang_validator_exe;
  // Path to the spirv-link executable that will be used to merge the individual spv files into a
  // single monolithic library.
  std::string_view spirv_link_exe;
};

class Module {
  std::vector<std::string> namespace_;

  std::vector<util::memory::Ref<type::Type>>                      type_list_;
  absl::flat_hash_map<std::string, util::memory::Ref<type::Type>> type_dict_;

  std::vector<util::memory::Ref<decl::VertexDeclaration>> vertex_function_list_;
  absl::flat_hash_map<std::string, util::memory::Ref<decl::VertexDeclaration>>
      vertex_function_dict_;

  std::vector<util::memory::Ref<decl::FragmentDeclaration>> fragment_function_list_;
  absl::flat_hash_map<std::string, util::memory::Ref<decl::FragmentDeclaration>>
      fragment_function_dict_;

  std::vector<util::memory::Ref<decl::PipelineDeclaration>>                      pipeline_list_;
  absl::flat_hash_map<std::string, util::memory::Ref<decl::PipelineDeclaration>> pipeline_dict_;

public:
  Module() = default;

  [[nodiscard]] const std::vector<util::memory::Ref<type::Type>>& types() const {
    return type_list_;
  }
  [[nodiscard]] const std::vector<util::memory::Ref<decl::VertexDeclaration>>& vertex_functions()
      const {
    return vertex_function_list_;
  }
  [[nodiscard]] const std::vector<util::memory::Ref<decl::FragmentDeclaration>>&
  fragment_functions() const {
    return fragment_function_list_;
  }

  void add_base_types();

  void set_namespace(std::vector<std::string>& ns) { namespace_ = ns; }

  void add_type(util::memory::Ref<type::Type> type) {
    type_list_.emplace_back(type);
    type_dict_.emplace(std::make_pair(type->name(), type));
  }

  void add_vertex_function(util::memory::Ref<decl::VertexDeclaration> decl) {
    vertex_function_list_.emplace_back(decl);
    vertex_function_dict_.emplace(std::make_pair(decl->name(), decl));
  }

  void add_fragment_function(util::memory::Ref<decl::FragmentDeclaration> decl) {
    fragment_function_list_.emplace_back(decl);
    fragment_function_dict_.emplace(std::make_pair(decl->name(), decl));
  }

  void add_pipeline(util::memory::Ref<decl::PipelineDeclaration> decl) {
    pipeline_list_.emplace_back(decl);
    pipeline_dict_.emplace(std::make_pair(decl->name(), decl));
  }

  [[nodiscard]] const std::optional<util::memory::Ref<type::Type>> find_type(
      std::string_view name) const;
  [[nodiscard]] const std::optional<util::memory::Ref<decl::VertexDeclaration>>
  find_vertex_function(std::string_view name) const;
  [[nodiscard]] const std::optional<util::memory::Ref<decl::FragmentDeclaration>>
  find_fragment_function(std::string_view name) const;

  void to_cpphdr(std::ostream& out, const CppOutputOptions& opts) const;
  void to_metal(std::ostream& out, const MetalOutputOptions& opts) const;
  void to_crystallib(std::ostream& out, const CrystallibOutputOptions& opts) const;

private:
  void make_vulkan_crystallib_(crystal::common::proto::Vulkan& vulkan_pb,
                               const std::string_view          glslang_validator_exe,
                               const std::string_view          spirv_link_exe) const;
  void make_metal_crystallib_(crystal::common::proto::Metal& metal_pb) const;
};

}  // namespace crystal::compiler::ast
