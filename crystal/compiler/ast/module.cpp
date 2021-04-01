#include "crystal/compiler/ast/module.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>

#include "crystal/common/proto/proto.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/type/all.hpp"
#include "util/fs/file.hpp"
#include "util/fs/tmp.hpp"
#include "util/msg/msg.hpp"
#include "util/proc/proc.hpp"

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

  auto texture2D_t = util::memory::Ref<type::StructType>::make("Texture2D", true);
  texture2D_t->set_glsl_name("sampler2D");
  texture2D_t->set_metal_name("texture2d<float>");

  add_type(float_t);
  add_type(vec2_t);
  add_type(vec3_t);
  add_type(vec4_t);
  add_type(mat4_t);
  add_type(texture2D_t);
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
    out << "};\n\n";
  }

  for (const auto& pipeline : pipeline_list_) {
    pipeline->to_metal(out, *this);
  }
}

void Module::to_crystallib(std::ostream& out, const CrystallibOutputOptions& opts) const {
  crystal::common::proto::Library lib_pb;

  if (opts.opengl) {
    for (const auto& pipeline : pipeline_list_) {
      pipeline->make_opengl_crystallib(*lib_pb.mutable_opengl()->add_pipelines(), *this);
    }
  }

  if (opts.vulkan) {
    make_vulkan_crystallib_(*lib_pb.mutable_vulkan(), opts.glslang_validator_exe,
                            opts.spirv_link_exe);
  }

  if (opts.metal) {
    make_metal_crystallib_(*lib_pb.mutable_metal());
  }

  if (!lib_pb.SerializeToOstream(&out)) {
    util::msg::fatal("serializing library to file");
  }
}

void Module::make_vulkan_crystallib_(crystal::common::proto::Vulkan& vulkan_pb,
                                     const std::string_view          glslang_validator_exe,
                                     const std::string_view          spirv_link_exe) const {
  const auto tmp_dir                    = util::fs::TemporaryDirectory();
  const auto glslang_validator_exe_path = std::filesystem::path{glslang_validator_exe};
  const auto spirv_link_exe_path        = std::filesystem::path{spirv_link_exe};

  std::vector<std::filesystem::path> spv_partials;
  for (const auto& pipeline : pipeline_list_) {
    {  // Vertex shader.
      const auto src_path = tmp_dir.path() / (pipeline->name() + ".vert.glsl");
      const auto spv_path = tmp_dir.path() / (pipeline->name() + ".vert.spv");

      {  // Wrap output file in additional scope to close file before running command on it.
        std::ofstream out(src_path);
        pipeline->vertex_function()->to_glsl(out, *this, true, true);
      }

      std::stringstream cmd;
      cmd << glslang_validator_exe_path << " -Os -V -S vert -e " << pipeline->name()
          << " --source-entrypoint main -o " << spv_path << " " << src_path;
      // std::cout << "VERT_CMD=" << cmd.str() << std::endl;
      util::proc::run_command(cmd.str().c_str());

      spv_partials.push_back(spv_path.string());
    }

    {  // Fragment shader.
      const auto src_path = tmp_dir.path() / (pipeline->name() + ".frag.glsl");
      const auto spv_path = tmp_dir.path() / (pipeline->name() + ".frag.spv");

      {  // Wrap output file in additional scope to close file before running command on it.
        std::ofstream out(src_path);
        pipeline->fragment_function()->to_glsl(out, *this, true, true);
      }

      std::stringstream cmd;
      cmd << glslang_validator_exe_path << " -Os -V -S frag -e " << pipeline->name()
          << " --source-entrypoint main -o " << spv_path << " " << src_path;
      // std::cout << "FRAG_CMD=" << cmd.str() << std::endl;
      util::proc::run_command(cmd.str().c_str());

      spv_partials.push_back(spv_path);
    }
  }

  {  // Link partial spv files together.
    const auto out_path = tmp_dir.path() / "_.spv";

    std::stringstream cmd;
    cmd << spirv_link_exe_path << " -o " << out_path;
    for (const auto& spv : spv_partials) {
      cmd << " " << spv;
    }
    // std::cout << "LINK_CMD=" << cmd.str() << std::endl;
    util::proc::run_command(cmd.str().c_str());

    const auto spv_contents = util::fs::read_file_binary(out_path.string());
    vulkan_pb.set_library(spv_contents.data(), spv_contents.size());
  }

  for (const auto& pipeline : pipeline_list_) {
    common::proto::VKPipeline* pipeline_pb = vulkan_pb.add_pipelines();

    pipeline_pb->set_name(pipeline->name());
    if (pipeline->fragment_function() != nullptr) {
      pipeline_pb->set_fragment(true);
    }

    for (const auto& [type, name, binding] : pipeline->uniforms()) {
      common::proto::VKUniform* uniform_pb = pipeline_pb->add_uniforms();
      uniform_pb->set_binding(binding);
    }

    for (const auto& [type, name, binding] : pipeline->textures()) {
      common::proto::VKTexture* texture_pb = pipeline_pb->add_textures();
      texture_pb->set_binding(binding);
    }
  }
}

void Module::make_metal_crystallib_(crystal::common::proto::Metal& metal_pb) const {
  const auto tmp_dir            = util::fs::TemporaryDirectory();
  auto       metal_file_name    = tmp_dir.path() / "tmp.metal";
  auto       air_file_name      = tmp_dir.path() / "tmp.air";
  auto       metallib_file_name = tmp_dir.path() / "tmp.metallib";

  {
    std::ofstream output_file(metal_file_name, std::ios::out);
    to_metal(output_file, MetalOutputOptions{});
  }

  {
    std::stringstream cmd;
    cmd << "xcrun -sdk macosx metal -c " << metal_file_name << " -o " << air_file_name;
    std::system(cmd.str().c_str());
  }

  {
    std::stringstream cmd;
    cmd << "xcrun -sdk macosx metallib " << air_file_name << " -o " << metallib_file_name;
    std::system(cmd.str().c_str());
  }

  const auto matallib_contents = util::fs::read_file_binary(metallib_file_name.string());
  metal_pb.set_library(matallib_contents.data(), matallib_contents.size());

  for (const auto& pipeline : pipeline_list_) {
    common::proto::MTLPipeline* pipeline_pb = metal_pb.add_pipelines();

    pipeline_pb->set_name(pipeline->name());
    pipeline_pb->set_vertex_name(pipeline->vertex_function()->name());
    if (pipeline->fragment_function() != nullptr) {
      pipeline_pb->set_fragment_name(pipeline->fragment_function()->name());
    }

    for (const auto& [type, name, binding] : pipeline->uniforms()) {
      common::proto::MTLUniform* uniform_pb = pipeline_pb->add_uniforms();
      uniform_pb->set_binding(binding);
      uniform_pb->set_actual(output::metal::uniform_binding(pipeline, binding));
    }
  }
}

}  // namespace crystal::compiler::ast
