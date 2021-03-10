#pragma once

#include <optional>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "crystal/compiler/ast/decl/fragment_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/type/type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast {

class Module {
  std::vector<std::string>                                                     namespace_;
  absl::flat_hash_map<std::string, util::memory::Ref<type::Type>>              types_;
  absl::flat_hash_map<std::string, util::memory::Ref<decl::VertexDeclaration>> vertex_functions_;
  absl::flat_hash_map<std::string, util::memory::Ref<decl::FragmentDeclaration>>
      fragment_functions_;

public:
  Module() = default;

  [[nodiscard]] const absl::flat_hash_map<std::string, util::memory::Ref<type::Type>>& types()
      const {
    return types_;
  }

  void add_base_types();

  void set_namespace(std::vector<std::string>& ns) { namespace_ = namespace_; }

  void add_type(util::memory::Ref<type::Type> type) {
    types_.emplace(std::make_pair(type->name(), type));
  }

  void add_vertex_function(util::memory::Ref<decl::VertexDeclaration> decl) {
    vertex_functions_.emplace(std::make_pair(decl->name(), decl));
  }

  void add_fragment_function(util::memory::Ref<decl::FragmentDeclaration> decl) {
    fragment_functions_.emplace(std::make_pair(decl->name(), decl));
  }

  [[nodiscard]] std::optional<util::memory::Ref<type::Type>> find_type(std::string_view name);
  [[nodiscard]] std::optional<util::memory::Ref<decl::VertexDeclaration>> find_vertex_function(
      std::string_view name);
  [[nodiscard]] std::optional<util::memory::Ref<decl::FragmentDeclaration>> find_fragment_function(
      std::string_view name);
};

}  // namespace crystal::compiler::ast
