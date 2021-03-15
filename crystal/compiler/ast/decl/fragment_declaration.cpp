#include "crystal/compiler/ast/decl/fragment_declaration.hpp"

#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

void FragmentDeclaration::to_glsl(std::ostream& out, const Module& mod) const {
  // Output the version header. This must come first.
  // out << "#version 420 core\n";
  out << output::VERSION_HDR;

  // TODO: Output only the used types.
  // Output the struct types.
  for (const auto& type : mod.types()) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << "{";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (const auto& prop : struct_type->properties()) {
      out << prop.type->name() << " " << prop.name << ";";
    }
    out << "};";
  }

  // Output the uniform blocks.
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Uniform) {
      continue;
    }

    // out << "layout(set=0, binding=" << input.index << ")uniform U" << input.index << "{";
    out << "uniform U" << input.index << "{";
    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (auto& prop : struct_type->properties()) {
      out << prop.type->name() << " " << prop.name << ";";
    }
    out << "}" << output::glsl_mangle_name{input.name} << ";";
  }

  // Output the input variables (vertex buffers).
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Varying) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (auto& prop : struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an input index.
        continue;
      }
      out << "layout(location=" << prop.index << ")in " << prop.type->name() << " "
          << output::glsl_varying_name{static_cast<uint32_t>(prop.index), prop.name} << ";";
    }
  }

  // Output the outputs.
  // This is the decomposed form of the return struct type from the fragment function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (const auto& prop : return_struct_type->properties()) {
    if (prop.index < 0) {
      // Skip properties that don't have an output index.
      continue;
    }

    out << "layout(location=" << prop.index << ")out " << prop.type->name() << " "
        << output::glsl_fragment_output_name{static_cast<uint32_t>(prop.index), prop.name} << ";";
  }

  // Finally output the function implementation.
  out << "void main(){";
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Varying) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    out << input.type->name() << " " << output::glsl_mangle_name{input.name} << ";";
    for (const auto& prop : struct_type->properties()) {
      out << output::glsl_mangle_name{input.name} << "." << prop.name << "="
          << output::glsl_varying_name{static_cast<uint32_t>(prop.index), prop.name} << ";";
    }
  }
  for (const auto& stmt : implementation_) {
    out << stmt->to_glsl(*this);
  }
  out << "}";
}

void FragmentDeclaration::to_pretty_glsl(std::ostream& out, const Module& mod) const {
  // Output the version header. This must come first.
  out << output::VERSION_HDR << "\n";

  // TODO: Output only the used types.
  // Output the struct types.
  for (const auto& type : mod.types()) {
    if (type->builtin()) {
      continue;
    }

    out << "struct " << type->name() << " {\n";
    const util::memory::Ref<type::StructType> struct_type = type;
    for (const auto& prop : struct_type->properties()) {
      out << "    " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "};\n";
  }

  // Output the uniform blocks.
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Uniform) {
      continue;
    }

    // out << "layout(set=0, binding=" << input.index << ") uniform U" << input.index << " {\n";
    out << "uniform U" << input.index << " {\n";
    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (const auto& prop : struct_type->properties()) {
      out << "    " << prop.type->name() << " " << prop.name << ";\n";
    }
    out << "} " << output::glsl_mangle_name{input.name} << ";\n";
  }

  out << "\n";

  // Output the input variables (vertex buffers).
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Varying) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (const auto& prop : struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an input index.
        continue;
      }

      out << "layout(location=" << prop.index << ") in " << prop.type->name() << " "
          << output::glsl_varying_name{static_cast<uint32_t>(prop.index), prop.name} << ";\n";
    }
  }

  out << "\n";

  // Output the outputs.
  // This is the decomposed form of the return struct type from the fragment function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (const auto& prop : return_struct_type->properties()) {
    if (prop.index < 0) {
      // Skip properties that don't have an output index.
      continue;
    }

    out << "layout(location=" << prop.index << ") out " << prop.type->name() << " "
        << output::glsl_fragment_output_name{static_cast<uint32_t>(prop.index), prop.name} << ";\n";
  }

  // Finally output the function implementation.
  out << "\nvoid main() {\n";
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Varying) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    out << "    " << input.type->name() << " " << output::glsl_mangle_name{input.name} << ";\n";
    for (const auto& prop : struct_type->properties()) {
      out << "    " << output::glsl_mangle_name{input.name} << "." << prop.name << " = "
          << output::glsl_varying_name{static_cast<uint32_t>(prop.index), prop.name} << ";\n";
    }
  }
  out << "\n";
  for (const auto& stmt : implementation_) {
    out << stmt->to_pretty_glsl(*this, 1);
  }
  out << "}\n";
}

}  // namespace crystal::compiler::ast::decl
