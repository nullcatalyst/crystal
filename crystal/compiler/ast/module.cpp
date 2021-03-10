#include "crystal/compiler/ast/module.hpp"

#include "crystal/compiler/ast/type/all.hpp"

namespace crystal::compiler::ast {

void Module::add_base_types() {
  const auto float_t = util::memory::Ref<type::StructType>::make("float", true);
  const auto vec2_t =
      util::memory::Ref<type::StructType>::make("vec2", true,
                                                std::vector{
                                                    type::StructProperty{"x", float_t},
                                                    type::StructProperty{"y", float_t},
                                                });
  const auto vec3_t =
      util::memory::Ref<type::StructType>::make("vec3", true,
                                                std::vector{
                                                    type::StructProperty{"x", float_t},
                                                    type::StructProperty{"y", float_t},
                                                    type::StructProperty{"z", float_t},
                                                });
  const auto vec4_t =
      util::memory::Ref<type::StructType>::make("vec4", true,
                                                std::vector{
                                                    type::StructProperty{"x", float_t},
                                                    type::StructProperty{"y", float_t},
                                                    type::StructProperty{"z", float_t},
                                                    type::StructProperty{"w", float_t},
                                                });

  add_type(float_t);
  add_type(vec2_t);
  add_type(vec3_t);
  add_type(vec4_t);
  add_type(util::memory::Ref<type::StructType>::make("mat4", true));
}

const std::optional<util::memory::Ref<type::Type>> Module::find_type(std::string_view name) const {
  const auto it = type_dict_.find(name);
  if (it != type_dict_.cend()) {
    return it->second;
  }

  return std::nullopt;
}

const std::optional<util::memory::Ref<decl::VertexDeclaration>> Module::find_vertex_function(
    std::string_view name) const {
  const auto it = vertex_function_dict_.find(name);
  if (it != vertex_function_dict_.cend()) {
    return it->second;
  }

  return std::nullopt;
}

const std::optional<util::memory::Ref<decl::FragmentDeclaration>> Module::find_fragment_function(
    std::string_view name) const {
  const auto it = fragment_function_dict_.find(name);
  if (it != fragment_function_dict_.cend()) {
    return it->second;
  }

  return std::nullopt;
}

void Module::to_cpphdr(std::ostream& out) const {
  out << "#pragma once\n\n"
      << "#include \"crystal/pipeline_desc.hpp\"\n"
      << "#include \"glm/glm.hpp\"\n\n";

  const auto cpp17 = true;

  // Output the start of the namespace.
  if (namespace_.size() > 0) {
    if (cpp17) {
      out << "namespace ";
      for (int i = 0; i < namespace_.size(); ++i) {
        if (i > 0) {
          out << "::";
        }
        out << namespace_[i];
      }
      out << " {\n\n";
    } else {
      for (int i = 0; i < namespace_.size(); ++i) {
        out << "namespace " << namespace_[i] << " {\n";
      }
      out << "\n";
    }
  }

  out << "using namespace glm;\n\n";

  // TODO: Output only the used types.
  // Output the struct types.
  for (const auto& type : type_list_) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << " {\n";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (auto& prop : struct_type->properties()) {
      out << "  " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "};\n\n";
  }

  for (const auto& pipeline : pipeline_list_) {
    pipeline->to_cpphdr(out, *this);
  }

  // Output the end of the namespace.
  if (namespace_.size() > 0) {
    if (cpp17) {
      out << "}  // namespace ";
      for (int i = 0; i < namespace_.size(); ++i) {
        if (i > 0) {
          out << "::";
        }
        out << namespace_[i];
      }
      out << "\n";
    } else {
      for (int i = 0; i < namespace_.size(); ++i) {
        out << "}  // namespace " << namespace_[i] << "\n";
      }
    }
  }
}

}  // namespace crystal::compiler::ast
