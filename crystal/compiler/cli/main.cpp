#include "cli11/cli11.hpp"
#include "crystal/common/proto/proto.hpp"
#include "crystal/compiler/parser/lexer.hpp"
#include "crystal/compiler/parser/parse.hpp"
#include "util/fs/path.hpp"

int main(const int argc, const char* const argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  using namespace crystal::compiler;

  cli::App app("App description");

  // {  // Convert to glsl.
  const auto glsl_cmd = app.add_subcommand("glsl");

  std::string glsl_input_file_name;
  glsl_cmd->add_option("-i,--input", glsl_input_file_name, "Input file name")->required();
  std::string glsl_output_directory;
  glsl_cmd->add_option("-o,--output", glsl_output_directory, "Output directory");
  bool pretty = false;
  glsl_cmd->add_flag("--pretty", pretty, "Output the shaders in a human readable form");

  glsl_cmd->final_callback([&]() {
    parser::Lexer lex = parser::Lexer::from_file(glsl_input_file_name);
    ast::Module   mod = parser::parse(lex);

    for (auto& vertex_function : mod.vertex_functions()) {
      const std::string output_file_name =
          util::fs::path_join(glsl_output_directory, vertex_function->name() + ".vert.glsl");
      std::ofstream output_file(output_file_name);
      util::msg::debug("outputting vertex file [", output_file_name, "]");

      vertex_function->to_glsl(output_file, mod, pretty);
    }

    for (auto& fragment_function : mod.fragment_functions()) {
      const std::string output_file_name =
          util::fs::path_join(glsl_output_directory, fragment_function->name() + ".frag.glsl");
      std::ofstream output_file(output_file_name);
      util::msg::debug("outputting fragment file [", output_file_name, "]");

      fragment_function->to_glsl(output_file, mod, pretty);
    }
  });
  // }

  // {  // Convert to cpp.
  const auto cpp_cmd = app.add_subcommand("cpp");

  std::string cpp_input_file_name;
  cpp_cmd->add_option("-i,--input", cpp_input_file_name, "Input file name")->required();
  std::string cpp_output_file_name;
  cpp_cmd->add_option("-o,--output", cpp_output_file_name, "Output file");

  cpp_cmd->final_callback([&]() {
    parser::Lexer lex = parser::Lexer::from_file(cpp_input_file_name);
    ast::Module   mod = parser::parse(lex);

    if (cpp_output_file_name.size() == 0) {
      cpp_output_file_name = util::fs::replace_extension(cpp_input_file_name, "crystal", "hpp");
    }

    std::ofstream output_file(cpp_output_file_name);
    util::msg::debug("outputting cpphdr file [", cpp_output_file_name, "]");
    mod.to_cpphdr(output_file, crystal::compiler::ast::CppOutputOptions{
                                   .cpp17 = true,
                               });
  });
  // }

  // {  // Convert to crystallib.
  const auto lib_cmd = app.add_subcommand("lib");

  std::string lib_input_file_name;
  lib_cmd->add_option("-i,--input", lib_input_file_name, "Input file name")->required();
  std::string lib_output_file_name;
  lib_cmd->add_option("-o,--output", lib_output_file_name, "Output file");

  lib_cmd->final_callback([&]() {
    parser::Lexer lex = parser::Lexer::from_file(lib_input_file_name);
    ast::Module   mod = parser::parse(lex);

    if (lib_output_file_name.size() == 0) {
      lib_output_file_name =
          util::fs::replace_extension(lib_input_file_name, "crystal", "crystallib");
    }

    std::ofstream output_file(lib_output_file_name, std::ios_base::out | std::ios_base::binary);
    util::msg::debug("outputting crystallib file [", lib_output_file_name, "]");
    mod.to_crystallib(output_file, crystal::compiler::ast::CrystallibOutputOptions{
                                       .glslang_validator_path = "glslangValidator",
                                       .spirv_link_path        = "spirv-link",
                                       .tmp                    = lib_output_file_name,
                                   });
  });
  // }

  CLI11_PARSE(app, argc, argv);
  return 0;
}
