#include "crystal/compiler/ast/decl/vertex_declaration.hpp"

#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

void VertexDeclaration::to_glsl(std::ostream& out, Module& mod) {
  // Output the version header. This must come first.
  out << "#version 420 core\n\n";

  // TODO: Output only the used types.
  // Output the struct types.
  for (auto& [type_name, type] : mod.types()) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << " {\n";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (auto& prop : struct_type->properties()) {
      out << "    " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "};\n";
  }

  // Output the uniform blocks.
  for (auto& input_buffer : input_buffers_) {
    if (input_buffer.buffer_type != decl::VertexInputBufferType::Uniform) {
      continue;
    }

    out << "layout(set=0, binding=" << input_buffer.index << ") uniform U" << input_buffer.index
        << " {\n";
    const util::memory::Ref<type::StructType> struct_type = input_buffer.type;
    for (auto& prop : struct_type->properties()) {
      out << "    " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "} _" << input_buffer.name << ";\n";
  }

  out << "\n";

  // Output the input variables (vertex buffers).
  for (auto& input_buffer : input_buffers_) {
    if (input_buffer.buffer_type != decl::VertexInputBufferType::Vertex &&
        input_buffer.buffer_type != decl::VertexInputBufferType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input_buffer.type;
    for (auto& prop : struct_type->properties()) {
      out << "layout(location=" << prop.index << ") in " << prop.type->name() << " i" << prop.index
          << "_" << prop.name << ";\n";
    }
  }

  out << "\n";

  // Output the varyings.
  // This is the decomposed form of the return struct type from the vertex function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (auto& prop : return_struct_type->properties()) {
    if (prop.index <= 0) {
      // Skip properties that don't have an output index.
      // The zero output for the vertex function must be the vertex position.
      continue;
    }

    out << "layout(location=" << (prop.index - 1) << ") out " << prop.type->name() << " o_"
        << prop.name << ";\n";
  }

  // Output the fixed line denoting the name of the vertex position variable. (Optional)
  out << "\nout gl_PerVertex { vec4 gl_Position; };\n\n";

  // Finally output the function implementation.
  out << "void main() {\n";
  for (auto& input_buffer : input_buffers_) {
    if (input_buffer.buffer_type != decl::VertexInputBufferType::Vertex &&
        input_buffer.buffer_type != decl::VertexInputBufferType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input_buffer.type;
    out << "    " << input_buffer.type->name() << " _" << input_buffer.name << ";\n";
    for (auto& prop : struct_type->properties()) {
      out << "    _" << input_buffer.name << "." << prop.name << " = i" << prop.index << "_"
          << prop.name << ";\n";
    }
  }
  out << "\n";
  for (auto stmt : implementation_) {
    stmt->to_glsl(out, *this, 1);
  }
  out << "}\n";
}

}  // namespace crystal::compiler::ast::decl
