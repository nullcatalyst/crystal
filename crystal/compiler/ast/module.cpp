#include "crystal/compiler/ast/module.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>

#include "crystal/common/proto/proto.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/type/all.hpp"
#include "util/fs/file.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast {

void Module::add_base_types() {
  auto float_t = util::memory::Ref<type::StructType>::make("float", true);
  float_t->set_glsl_name("float");
  float_t->set_metal_name("float");
  auto vec2_t = util::memory::Ref<type::StructType>::make("vec2", true,
                                                          std::vector{
                                                              type::StructProperty{"x", float_t},
                                                              type::StructProperty{"y", float_t},
                                                          });
  vec2_t->set_glsl_name("vec2");
  vec2_t->set_metal_name("float2");
  auto vec3_t = util::memory::Ref<type::StructType>::make("vec3", true,
                                                          std::vector{
                                                              type::StructProperty{"x", float_t},
                                                              type::StructProperty{"y", float_t},
                                                              type::StructProperty{"z", float_t},
                                                          });
  vec3_t->set_glsl_name("vec3");
  vec3_t->set_metal_name("float3");
  auto vec4_t = util::memory::Ref<type::StructType>::make("vec4", true,
                                                          std::vector{
                                                              type::StructProperty{"x", float_t},
                                                              type::StructProperty{"y", float_t},
                                                              type::StructProperty{"z", float_t},
                                                              type::StructProperty{"w", float_t},
                                                          });
  vec4_t->set_glsl_name("vec4");
  vec4_t->set_metal_name("float4");
  auto mat4_t = util::memory::Ref<type::StructType>::make("mat4", true);
  mat4_t->set_glsl_name("mat4");
  mat4_t->set_metal_name("float4x4");

  add_type(float_t);
  add_type(vec2_t);
  add_type(vec3_t);
  add_type(vec4_t);
  add_type(mat4_t);
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

void Module::to_cpphdr(std::ostream& out, const CppOutputOptions& opts) const {
  out << "#pragma once\n\n"
      << "#include \"crystal/common/pipeline_desc.hpp\"\n"
      << "#include \"glm/glm.hpp\"\n\n";

  // Output the start of the namespace.
  if (namespace_.size() > 0) {
    if (opts.cpp17) {
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
    if (opts.cpp17) {
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

void Module::to_metal(std::ostream& out, const MetalOutputOptions& opts) const {
  out << output::metal::HDR;

  // Output the struct types.
  for (const auto& type : type_list_) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << " {\n";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (auto& prop : struct_type->properties()) {
      out << output::metal::indent{1} << prop.type->metal_name() << " " << prop.name;
      if (prop.index >= 0) {
        out << " [[ color(" << prop.index << ") ]]";
      }
      out << ";\n";
    }
    out << "};\n";

    // {  // Uniform type.
    //   out << "struct " << type->name() << "_u {\n";
    //   const util::memory::Ref<type::StructType> struct_type = type;
    //   for (auto& prop : struct_type->properties()) {
    //     out << output::metal::indent{1} << prop.type->metal_name() << " " << prop.name << ";\n";
    //   }
    //   out << "};\n";
    // }
    // {
    //   // Varying type.
    //   out << "struct " << type->name() << "_v {\n";
    //   const util::memory::Ref<type::StructType> struct_type = type;
    //   for (auto& prop : struct_type->properties()) {
    //     out << output::metal::indent{1} << prop.type->metal_name() << " " << prop.name;
    //     if (prop.index == 0) {
    //       out << " [[ position ]]";
    //     }
    //     out << ";\n";
    //   }
    //   out << "};\n";
    // }
    // {  // Output type.
    //   out << "struct " << type->name() << "_o {\n";
    //   const util::memory::Ref<type::StructType> struct_type = type;
    //   for (auto& prop : struct_type->properties()) {
    //     if (prop.index < 0) {
    //       continue;
    //     }
    //     out << output::metal::indent{1} << prop.type->metal_name() << " " << prop.name
    //         << " [[ color(" << prop.index << ") ]];\n";
    //   }
    //   out << "};\n";
    // }

    out << "\n";
  }

  for (const auto& pipeline : pipeline_list_) {
    pipeline->to_metal(out, *this);
  }
}

void Module::to_crystallib(std::ostream& out, const CrystallibOutputOptions& opts) const {
  crystal::common::proto::Library lib_pb;

  {  // OpenGL.
    for (const auto& pipeline : pipeline_list_) {
      pipeline->to_crystallib(*lib_pb.mutable_opengl()->add_pipelines(), *this);
    }
  }

  {  // Metal.
    char       buffer[]           = "/var/tmp/crystal_XXXXXX";
    const auto tmp_dir            = std::string(mkdtemp(buffer));
    auto       metal_file_name    = tmp_dir + "/a.metal";
    auto       air_file_name      = tmp_dir + "/a.air";
    auto       metallib_file_name = tmp_dir + "/a.metallib";

    std::cout << "metal_file_name=" << metal_file_name << std::endl;
    std::cout << "air_file_name=" << air_file_name << std::endl;
    std::cout << "metallib_file_name=" << metallib_file_name << std::endl;

    {
      std::ofstream output_file(metal_file_name, std::ios::out);
      to_metal(output_file, MetalOutputOptions{});
    }

    {
      std::stringstream cmd;
      cmd << "xcrun -sdk macosx metal -c " << metal_file_name << " -o " << air_file_name;
      std::system(cmd.str().c_str());
      // std::remove(metal_file_name.c_str());
    }

    {
      std::stringstream cmd;
      cmd << "xcrun -sdk macosx metallib " << air_file_name << " -o " << metallib_file_name;
      std::system(cmd.str().c_str());
      // std::remove(air_file_name.c_str());
    }

    const auto matallib_contents = util::fs::read_file_binary(metallib_file_name);
    // std::remove(metallib_file_name.c_str());

    common::proto::Metal* metal_pb = lib_pb.mutable_metal();
    metal_pb->set_library(matallib_contents.data(), matallib_contents.size());

    for (const auto& pipeline : pipeline_list_) {
      common::proto::MTLPipeline* pipeline_pb = metal_pb->add_pipelines();

      pipeline_pb->set_name(pipeline->name());
      pipeline_pb->set_vertex_name(pipeline->vertex_function()->name());
      if (pipeline->fragment_function() != nullptr) {
        pipeline_pb->set_fragment_name(pipeline->fragment_function()->name());
      }
    }
  }

  if (!lib_pb.SerializeToOstream(&out)) {
    util::msg::fatal("serializing library to file");
  }
}

}  // namespace crystal::compiler::ast
