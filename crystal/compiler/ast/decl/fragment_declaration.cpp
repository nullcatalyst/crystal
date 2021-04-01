#include "crystal/compiler/ast/decl/fragment_declaration.hpp"

#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

void FragmentDeclaration::to_glsl(std::ostream& out, const Module& mod, bool pretty,
                                  bool vulkan) const {
  const output::glsl::Options opts{mod, nullptr, this, 0, pretty, vulkan};

  // Output the version header. This must come first.
  out << (opts.vulkan ? output::glsl::VK_HDR : output::glsl::GL_HDR);
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
    if (input.input_type != decl::FragmentInputType::Uniform) {
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

  // Output the textures.
  for (const auto& input : inputs_) {
    if (input.input_type != decl::FragmentInputType::Texture) {
      continue;
    }

    {
      out << output::glsl::indent{opts.indent};
      if (opts.vulkan) {
        out << "layout(set=1, binding=" << input.index << (opts.pretty ? ") " : ")");
      }
      out << "uniform " << input.type->glsl_name() << " " << output::glsl::mangle_name{input.name}
          << (opts.pretty ? ";\n" : ";");
    }
  }

  if (opts.pretty) {
    out << "\n";
  }

  // Output the varyings.
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
      out << output::glsl::indent{opts.indent} << "layout(location=" << prop.index
          << (opts.pretty ? ") in " : ")in ") << prop.type->name() << " "
          << output::glsl::varying_name{static_cast<uint32_t>(prop.index), prop.name}
          << (opts.pretty ? ";\n" : ";");
    }
    break;
  }

  if (opts.pretty) {
    out << "\n";
  }

  // Output the outputs.
  // This is the decomposed form of the return struct type from the fragment function.
  const util::memory::Ref<type::StructType> return_struct_type = return_type_;
  for (const auto& prop : return_struct_type->properties()) {
    if (prop.index < 0) {
      // Skip properties that don't have an output index.
      continue;
    }

    out << output::glsl::indent{opts.indent} << "layout(location=" << prop.index
        << (opts.pretty ? ") out " : ")out ") << prop.type->name() << " "
        << output::glsl::fragment_output_name{static_cast<uint32_t>(prop.index), prop.name}
        << (opts.pretty ? ";\n" : ";");
  }

  if (opts.pretty) {
    out << "\n";
  }

  {  // Finally output the function implementation.
    out << output::glsl::indent{opts.indent} << "void main()" << (opts.pretty ? " {\n" : "{");

    const auto main_opts = opts.incr_indent();
    for (const auto& input : inputs_) {
      if (input.input_type != decl::FragmentInputType::Varying) {
        continue;
      }

      const util::memory::Ref<type::StructType> struct_type = input.type;
      out << output::glsl::indent{main_opts.indent} << input.type->name() << " "
          << output::glsl::mangle_name{input.name} << (main_opts.pretty ? ";\n" : ";");
      for (const auto& prop : struct_type->properties()) {
        out << output::glsl::indent{main_opts.indent} << output::glsl::mangle_name{input.name}
            << "." << prop.name << (main_opts.pretty ? " = " : "=")
            << output::glsl::varying_name{static_cast<uint32_t>(prop.index), prop.name}
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

void FragmentDeclaration::to_metal(std::ostream& out, const Module& mod) const {
  const output::metal::Options opts{mod, nullptr, this, 0};

  // Output the function implementation.
  out << "fragment " << return_type_->metal_name() << " " << name() << "(";

  bool first = true;
  for (const auto& input : inputs_) {
    if (!first) {
      out << ", ";
    } else {
      first = false;
    }

    if (input.input_type == decl::FragmentInputType::Varying) {
      std::string short_name = name().substr(0, name().size() - 5);
      out << short_name << "_v " << output::metal::mangle_name{input.name} << " [[ stage_in ]]";
    }
    if (input.input_type == decl::FragmentInputType::Uniform) {
      out << "constant " << input.type->metal_name() << "& "
          << output::metal::mangle_name{input.name} << " [[ buffer(" << input.index << ") ]]";
    }
    if (input.input_type == decl::FragmentInputType::Texture) {
      out << "texture2d<float> " << output::metal::mangle_name{input.name} << " [[ texture("
          << input.index << ") ]], "
          << "sampler " << output::metal::mangle_name{input.name + "_sampler"} << " [[ sampler("
          << input.index << ") ]]";
    }
  }
  out << ") {\n";
  for (const auto& stmt : implementation_) {
    out << stmt->to_metal(opts);
  }
  out << "}\n";
}

}  // namespace crystal::compiler::ast::decl
