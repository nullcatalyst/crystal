#include "crystal/compiler/ast/decl/vertex_declaration.hpp"

#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

void VertexDeclaration::to_glsl(std::ostream& out, const Module& mod) const {
  // Output the version header. This must come first.
  out << "#version 420 core\n";

  // TODO: Output only the used types.
  // Output the struct types.
  for (auto& type : mod.types()) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << "{";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (auto& prop : struct_type->properties()) {
      out << prop.type->name() << " " << prop.name << ";";
    }
    out << "};";
  }

  // Output the uniform blocks.
  for (auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Uniform) {
      continue;
    }

    out << "layout(set=0, binding=" << input.index << ")uniform U" << input.index << "{";
    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (auto& prop : struct_type->properties()) {
      out << prop.type->name() << " " << prop.name << ";";
    }
    out << "}" << output::glsl_mangle_name{input.name} << ";";
  }

  // Output the input variables (vertex buffers).
  for (auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Vertex &&
        input.input_type != decl::VertexInputType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (auto& prop : struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an input index.
        continue;
      }
      out << "layout(location=" << prop.index << ")in " << prop.type->name() << " i" << prop.index
          << output::glsl_mangle_name{prop.name} << ";";
    }
  }

  // Output the varyings.
  // This is the decomposed form of the return struct type from the vertex function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (auto& prop : return_struct_type->properties()) {
    if (prop.index < 0) {
      // Skip properties that don't have an output index.
      continue;
    }
    out << "layout(location=" << prop.index << ")out " << prop.type->name() << " o_" << prop.name
        << ";";
  }

  // Output the fixed line denoting the name of the vertex position variable. (Optional)
  // out << "out gl_PerVertex{vec4 gl_Position;};";

  // Finally output the function implementation.
  out << "void main(){";
  for (auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Vertex &&
        input.input_type != decl::VertexInputType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    out << input.type->name() << " " << output::glsl_mangle_name{input.name} << ";";
    for (auto& prop : struct_type->properties()) {
      out << output::glsl_mangle_name{input.name} << "." << prop.name << "=i" << prop.index << "_"
          << prop.name << ";";
    }
  }
  for (auto stmt : implementation_) {
    out << stmt->to_glsl(*this);
  }
  out << "}";
}

void VertexDeclaration::to_pretty_glsl(std::ostream& out, const Module& mod) const {
  // Output the version header. This must come first.
  out << "#version 420 core\n\n";

  // TODO: Output only the used types.
  // Output the struct types.
  for (auto& type : mod.types()) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << " {\n";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (auto& prop : struct_type->properties()) {
      out << "    " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "};\n\n";
  }

  // Output the uniform blocks.
  for (auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Uniform) {
      continue;
    }

    out << "layout(set=0, binding=" << input.index << ") uniform U" << input.index << " {\n";
    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (auto& prop : struct_type->properties()) {
      out << "    " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "} _" << input.name << ";\n";
  }

  out << "\n";

  // Output the input variables (vertex buffers).
  for (auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Vertex &&
        input.input_type != decl::VertexInputType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (auto& prop : struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an input index.
        continue;
      }
      out << "layout(location=" << prop.index << ") in " << prop.type->name() << " i" << prop.index
          << "_" << prop.name << ";\n";
    }
  }

  out << "\n";

  // Output the varyings.
  // This is the decomposed form of the return struct type from the vertex function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (auto& prop : return_struct_type->properties()) {
    if (prop.index < 0) {
      // Skip properties that don't have an output index.
      continue;
    }

    out << "layout(location=" << prop.index << ") out " << prop.type->name() << " o_" << prop.name
        << ";\n";
  }

  // Output the fixed line denoting the name of the vertex position variable. (Optional)
  out << "\nout gl_PerVertex { vec4 gl_Position; };\n\n";

  // Finally output the function implementation.
  out << "void main() {\n";
  for (auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Vertex &&
        input.input_type != decl::VertexInputType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    out << "    " << input.type->name() << " " << output::glsl_mangle_name{input.name} << ";\n";
    for (auto& prop : struct_type->properties()) {
      out << "    " << output::glsl_mangle_name{input.name} << "." << prop.name << " = i"
          << prop.index << "_" << prop.name << ";\n";
    }
  }
  out << "\n";
  for (auto stmt : implementation_) {
    out << stmt->to_pretty_glsl(*this, 1);
  }
  out << "}\n";
}

}  // namespace crystal::compiler::ast::decl
