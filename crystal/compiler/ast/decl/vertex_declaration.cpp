#include "crystal/compiler/ast/decl/vertex_declaration.hpp"

#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

void VertexDeclaration::to_glsl(std::ostream& out, const Module& mod, bool pretty) const {
  const output::glsl::Options opts{mod, this, nullptr, 0, pretty, false};

  // Output the version header. This must come first.
  out << output::glsl::HDR;
  if (opts.pretty) {
    out << "\n";
  }

  // TODO: Output only the used types.
  // Output the struct types.
  for (const auto& type : mod.types()) {
    if (type->builtin()) {
      continue;
    }

    {
      out << "struct " << type->name() << (opts.pretty ? " {\n" : "{");
      const util::memory::Ref<type::StructType> struct_type = type;
      const auto                                struct_opts = opts.incr_indent();
      for (const auto& prop : struct_type->properties()) {
        out << output::glsl::indent{struct_opts.indent} << prop.type->name() << " " << prop.name
            << (struct_opts.pretty ? ";\n" : ";");
      }
      out << (opts.pretty ? "};\n\n" : "};");
    }
  }

  // Output the uniform blocks.
  for (const auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Uniform) {
      continue;
    }

    {
      out << output::glsl::indent{opts.indent};
      if (opts.vulkan) {
        out << "layout(set=0, binding=" << input.index << (opts.pretty ? ") " : ")");
      }
      out << "uniform U" << input.index << (opts.pretty ? " {\n" : "{");
      const util::memory::Ref<type::StructType> struct_type = input.type;
      const auto                                struct_opts = opts.incr_indent();
      for (const auto& prop : struct_type->properties()) {
        out << output::glsl::indent{struct_opts.indent} << prop.type->name() << " " << prop.name
            << (struct_opts.pretty ? ";\n" : ";");
      }
      out << output::glsl::indent{opts.indent} << (opts.pretty ? "} " : "}")
          << output::glsl::mangle_name{input.name} << (opts.pretty ? ";\n" : ";");
    }
  }

  if (opts.pretty) {
    out << "\n";
  }

  // Output the input variables (vertex buffers).
  for (const auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Vertex &&
        input.input_type != decl::VertexInputType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    for (const auto& prop : struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an input index.
        continue;
      }
      out << output::glsl::indent{opts.indent} << "layout(location=" << prop.index
          << (opts.pretty ? ") in " : ")in ") << prop.type->name() << " "
          << output::glsl::vertex_input_name{static_cast<uint32_t>(prop.index), prop.name}
          << (opts.pretty ? ";\n" : ";");
    }
  }

  if (opts.pretty) {
    out << "\n";
  }

  // Output the varyings.
  // This is the decomposed form of the return struct type from the vertex function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (const auto& prop : return_struct_type->properties()) {
    if (prop.index < 0) {
      // Skip properties that don't have an output index.
      continue;
    }

    out << output::glsl::indent{opts.indent} << "layout(location=" << prop.index
        << (opts.pretty ? ") out " : ")out ") << prop.type->name() << " "
        << output::glsl::varying_name{static_cast<uint32_t>(prop.index), prop.name}
        << (opts.pretty ? ";\n" : ";");
  }

  if (opts.pretty) {
    out << "\n";
  }

  // Output the fixed line denoting the name of the vertex position variable. (Optional)
  // out << "\nout gl_PerVertex { vec4 gl_Position; };\n\n";

  {  // Finally output the function implementation.
    out << output::glsl::indent{opts.indent} << "void main()" << (opts.pretty ? " {\n" : "{");

    const auto main_opts = opts.incr_indent();
    for (const auto& input : inputs_) {
      if (input.input_type != decl::VertexInputType::Vertex &&
          input.input_type != decl::VertexInputType::Instanced) {
        continue;
      }

      const util::memory::Ref<type::StructType> struct_type = input.type;
      out << output::glsl::indent{main_opts.indent} << input.type->name() << " "
          << output::glsl::mangle_name{input.name} << (main_opts.pretty ? ";\n" : ";");
      for (const auto& prop : struct_type->properties()) {
        out << output::glsl::indent{main_opts.indent} << output::glsl::mangle_name{input.name}
            << "." << prop.name << (main_opts.pretty ? " = " : "=")
            << output::glsl::vertex_input_name{static_cast<uint32_t>(prop.index), prop.name}
            << (main_opts.pretty ? ";\n" : ";");
      }
    }
    if (main_opts.pretty) {
      out << "\n";
    }
    for (const auto& stmt : implementation_) {
      out << stmt->to_glsl(main_opts);
    }

    out << (opts.pretty ? "}\n" : "}");
  }
}

void VertexDeclaration::to_metal(std::ostream& out, const Module& mod) const {
  const output::metal::Options opts{mod, this, nullptr, 0};

  std::string short_name = name().substr(0, name().size() - 5);

  {  // Input type.
    out << "struct " << short_name << "_i {\n";

    for (const auto& input : inputs_) {
      if (input.input_type != decl::VertexInputType::Vertex &&
          input.input_type != decl::VertexInputType::Instanced) {
        continue;
      }

      const util::memory::Ref<type::StructType> struct_type = input.type;
      for (const auto& prop : struct_type->properties()) {
        if (prop.index < 0) {
          // Skip properties that don't have an input index.
          continue;
        }
        out << output::metal::indent{1} << prop.type->metal_name() << " " << prop.name
            << " [[ attribute(" << prop.index << ") ]];\n";
      }
    }
    out << "};\n";
  }
  {  // Varying type.
    out << "struct " << short_name << "_v {\n";
    const util::memory::Ref<type::StructType> struct_type = return_type_;
    for (auto& prop : struct_type->properties()) {
      out << output::metal::indent{1} << prop.type->metal_name() << " " << prop.name;
      if (prop.index == 0) {
        out << " [[ position ]]";
      }
      out << ";\n";
    }
    out << "};\n";
  }

  // Output the function implementation.
  out << "vertex " << short_name << "_v " << name() << "(" << short_name << "_i in [[ stage_in ]]";
  for (const auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Uniform) {
      continue;
    }
    out << ", constant " << input.type->metal_name() << "& "
        << output::metal::mangle_name{input.name} << " [[ buffer(" << input.index << ") ]]";
  }
  out << ") {\n";
  for (const auto& input : inputs_) {
    if (input.input_type != decl::VertexInputType::Vertex &&
        input.input_type != decl::VertexInputType::Instanced) {
      continue;
    }

    const util::memory::Ref<type::StructType> struct_type = input.type;
    out << output::metal::indent{1} << input.type->name() << " "
        << output::glsl::mangle_name{input.name} << ";\n";
    for (const auto& prop : struct_type->properties()) {
      out << output::metal::indent{1} << output::metal::mangle_name{input.name} << "." << prop.name
          << " = " << output::metal::vertex_input_name{static_cast<uint32_t>(prop.index), prop.name}
          << ";\n";
    }
  }
  out << "\n";
  for (const auto& stmt : implementation_) {
    out << stmt->to_metal(opts);
  }
  out << "}\n";
}

}  // namespace crystal::compiler::ast::decl
