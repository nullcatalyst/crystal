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

std::optional<util::memory::Ref<type::Type>> Module::find_type(std::string_view name) {
  const auto it = types_.find(name);
  if (it != types_.cend()) {
    return it->second;
  }

  return std::nullopt;
}

std::optional<util::memory::Ref<decl::VertexDeclaration>> Module::find_vertex_function(
    std::string_view name) {
  const auto it = vertex_functions_.find(name);
  if (it != vertex_functions_.cend()) {
    return it->second;
  }

  return std::nullopt;
}

}  // namespace crystal::compiler::ast
